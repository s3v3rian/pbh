#include "denm_mngr.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/inc/error_code_user.h"

/*
 *******************************************************************************
 * Private function signatures
 *******************************************************************************
 */

static int32_t denm_mngr_msg_init(SITSStationInfo *psStationInfo, DENM *psOutputDenm);
static int32_t denm_mngr_msg_encode(uint8_t **p2un8DenmPayload, fix_data_t *psPotiFixData, DENM *psOutputDenm, ITSMsgCodecErr *psOutputErr);
static int32_t denm_mngr_msg_decode(uint8_t *pun8RxPayload, int32_t n32RxPayloadLength, btp_handler_recv_indicator_t *psBtpRecvStatus, bool bSspCheck, DENM *psOutputDenm, ITSMsgCodecErr *psOutputErr);
static int32_t denm_mngr_check_msg_permissions(DENM *psOutputDenm, uint8_t *pun8Ssp, uint32_t un32SspLength);

/*
 *******************************************************************************
 * Private/Extern variables
 *******************************************************************************
 */

// BTP fields.
btp_handler_ptr m_pBtpDenmHandler;
btp_handler_send_config_t m_sBtpDenmSendConfig;

// DENM generation fields.
ITSMsgCodecErr m_sEncodeDenmErr;

// DENM decode fields.
uint8_t m_aun8DenmRxPayload[GN_MAX_SDU_SIZE];
ITSMsgCodecErr m_sDecodeDenmErr;
btp_handler_recv_indicator_t m_sBtpDenmRecvStatus;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t denm_mngr_init() {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    uint16_t un16DenmSrcPrt = DENM_BTP_PORT; /* Listen DENM message from remote ITS station. */
    uint16_t un16DenmDestPrt = DENM_BTP_PORT; /* Broadcast DENM message to remote ITS station. */

    btp_handler_config_t sBtpDenmConfig;

    BTP_HDLR_CONFIG_INIT(&sBtpDenmConfig, un16DenmDestPrt);

#if (__EN_SECURITY_FEATURE__)

    /*
     * Enable AID security mode.
     * Please make sure all the security configuration in gnd.json is set correctly.
     */
    sBtpDenmConfig.security.type = GN_SEC_ENCAP_PKT_BY_AID_PROFILE;
    sBtpDenmConfig.security.its_aid = ITS_SEC_DEN_AID;

#endif

    // Create BTP handler.
    n32Result = btp_create(&m_pBtpDenmHandler, &sBtpDenmConfig);

    if(false == IS_SUCCESS(n32Result)) {

        printf("Cannot create BTP handler: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    /* Bind BTP port. */
    n32Result = btp_bind(m_pBtpDenmHandler, un16DenmSrcPrt);

    if(false == IS_SUCCESS(n32Result)) {

        printf("Cannot bind BTP handler: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    /* BTP send packet default configuration (SHB/BTP-B). */
    BTP_SEND_CONFIG_INIT(&m_sBtpDenmSendConfig);

#if (__EN_SECURITY_FEATURE__)

    /*
    * Assign DENM service specific permissions according to the actual content in payload.
    * Please refer to ETSI TS 103 097, ETSI EN 302 637-2 for more details.
    * Please refer to Unex-APG-ETSI-GN-BTP for more information of build-in certificates
    */

    /* SSP Version control */
    m_sBtpDenmSendConfig.security.ssp[0] = 0;
    /* Service-specific parameter : CauseCodeType */
    m_sBtpDenmSendConfig.security.ssp[1] = 0;
    m_sBtpDenmSendConfig.security.ssp[2] = WRONG_WAY_DRIVING;
    m_sBtpDenmSendConfig.security.ssp[3] = 0;
    m_sBtpDenmSendConfig.security.ssp_len = 4;

#endif

    /* Allocate a buffer for restoring the decode error information if needed. */
    m_sDecodeDenmErr.msg_size = 256;
    m_sDecodeDenmErr.msg = calloc(1, m_sDecodeDenmErr.msg_size);

    /* Allocate a buffer for restoring the decode error information if needed. */
    m_sEncodeDenmErr.msg_size = 256;
    m_sEncodeDenmErr.msg = calloc(1, m_sEncodeDenmErr.msg_size);

    if(NULL == m_sEncodeDenmErr.msg
            || NULL == m_sDecodeDenmErr.msg) {

        printf("Cannot allocate memory for DENM error message buffer.\n");

        n32Result = PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    return n32Result;
}

int32_t denm_mngr_process_tx(SITSStationInfo *psStationInfo, fix_data_t *psPotiFixData, DENM *psOutputDenm) {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    uint8_t *pun8TxPayload = NULL;
    int32_t n32TxPayloadSize = 0;

    if(NULL == psOutputDenm
            || NULL == psPotiFixData) {

        printf("Failed to process denm tx\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    //printf("Preparing DENM message\n");

    // Init denm message.
    denm_mngr_msg_init(psStationInfo, psOutputDenm);

    // Generate DENM message.
    n32TxPayloadSize = denm_mngr_msg_encode(
                &pun8TxPayload,
                psPotiFixData,
                psOutputDenm,
                &m_sEncodeDenmErr);

    if(pun8TxPayload == NULL
            || n32TxPayloadSize <= 0) {

        printf("Failed to generate DENM message - Err: %s\n", m_sEncodeDenmErr.msg);
        return PROCEDURE_INVALID_SERVICE_TX_ERROR;
    }

    //printf("Setting special DENM parameters\n");

    /* BTP send packet default configuration (GBC/BTP-B). */
    BTP_SEND_CONFIG_INIT(&m_sBtpDenmSendConfig);

    /* Must transmit DENM by using GBC mechanism, so set TX network header type to GBC. */
    GN_ENABLE_OPTIONAL_CONFIG(m_sBtpDenmSendConfig.gn_optional.bitmask, GN_OPTION_NETWORK_PKT_TYPE_BIT);
    m_sBtpDenmSendConfig.gn_optional.type = GN_SVC_NETWORK_PKT_GBC;

    /* Setup destination area according to current position. */
    GN_ENABLE_OPTIONAL_CONFIG(m_sBtpDenmSendConfig.gn_optional.bitmask, GN_OPTION_DEST_AREA_BIT);
    m_sBtpDenmSendConfig.gn_optional.dest_area.area_type = GN_AREA_TYPE_RECT; /* GN_AREA_TYPE_CIRCLE, GN_AREA_TYPE_RECT, GN_AREA_TYPE_ELIP. */
    m_sBtpDenmSendConfig.gn_optional.dest_area.pos_latitude = (int32_t)(psPotiFixData->latitude * 10000000.0); /* WGS84, 1/10 micros degrees. */
    m_sBtpDenmSendConfig.gn_optional.dest_area.pos_longitude = (int32_t)(psPotiFixData->longitude * 10000000.0); /* WGS84, 1/10 micros degrees. */
    m_sBtpDenmSendConfig.gn_optional.dest_area.distance_a = 100; /* Distance a of the geometric shape, meters. */
    m_sBtpDenmSendConfig.gn_optional.dest_area.distance_b = 100; /* Distance b of the geometric shape, meters. */
    m_sBtpDenmSendConfig.gn_optional.dest_area.angle = 0; /* Angle of the geometric shape, degrees from North. */

    //printf("Sending DENM message\n");

    /* Broadcast the DENM message (BTP-B, SHB). */
    n32Result = btp_send(m_pBtpDenmHandler, &m_sBtpDenmSendConfig, pun8TxPayload, n32TxPayloadSize);

    if(0 >= n32Result) {

        printf("Transmitting error: %s\n", ERROR_MSG(n32Result));
    }

    // Release the allocated message buffer.
    itsmsg_buf_free(pun8TxPayload);

    return n32Result;
}

int32_t denm_mngr_process_rx(DENM *psOutputDenm) {

    bool bSspCheck = false;

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    /* Listen the CAM BTP port. */
    n32Result = btp_recv(m_pBtpDenmHandler, &m_sBtpDenmRecvStatus, m_aun8DenmRxPayload, sizeof(m_aun8DenmRxPayload), BTP_RECV_WAIT_FOREVER);

    if(0 > n32Result > 0) {

        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    if(true == IS_DECAP_FAIL(m_sBtpDenmRecvStatus.security.status)) {

        printf("\tSecurity status: decapsulation error (%d), the payload content is untrustworthy\n", m_sBtpDenmRecvStatus.security.status);
        return PROCEDURE_SECURITY_ERROR;
    }

    if(ITS_SEC_SUCCESS == m_sBtpDenmRecvStatus.security.status) {

        bSspCheck = true;

        /* Try to decode the received message. */
        denm_mngr_msg_decode(
                    m_aun8DenmRxPayload,
                    n32Result,
                    &m_sBtpDenmRecvStatus,
                    bSspCheck,
                    psOutputDenm,
                    &m_sDecodeDenmErr);

    } else if(m_sBtpDenmRecvStatus.security.status == ITS_SEC_NA) {

        printf("\tSecurity status: no security protection\n");

    } else {

        printf("\tSecurity status: other (%d)\n", m_sBtpDenmRecvStatus.security.status);
    }

    return PROCEDURE_SUCCESSFULL;
}

int32_t denm_mngr_release() {

    btp_release(m_pBtpDenmHandler);

    return PROCEDURE_SUCCESSFULL;
}

void denm_mngr_printf_denm(DENM *psDenm) {

    int32_t n32SentenceSize = 0;
    char achSentence[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    memset(achSentence, 0, MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES);

    // Always write station id first.
    n32SentenceSize += snprintf(
                achSentence + n32SentenceSize,
                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                "T%d",
                psDenm->header.stationID);

    // Write to boundary current situation event.
    if(TRUE == psDenm->denm.situation_option) {

        switch(psDenm->denm.situation.eventType.causeCode) {

            case CauseCodeType_commercialVehicleSituation:

                n32SentenceSize += snprintf(
                            achSentence + n32SentenceSize,
                            MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                            ",Event,commercial_vehicle_status");
                break;

            case CauseCodeType_signalViolation:

                if(SignalViolation_trafficLightViolation == psDenm->denm.situation.eventType.subCauseCode) {

                    n32SentenceSize += snprintf(
                                achSentence + n32SentenceSize,
                                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                                ",Event,signal_violation");
                }
                break;

        case CauseCodeType_slowVehicle:

                n32SentenceSize += snprintf(
                            achSentence + n32SentenceSize,
                            MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                            ",Event,slow_vehicle");
                break;

            default:

                n32SentenceSize += snprintf(
                            achSentence + n32SentenceSize,
                            MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                            ",Event,%d", psDenm->denm.situation.eventType.causeCode);
                break;
        }

    } else {

        n32SentenceSize += snprintf(
                    achSentence + n32SentenceSize,
                    MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                    ",No event");
    }

    // Write to boundary alacarte container stuff.
    if(TRUE == psDenm->denm.alacarte_option) {

        n32SentenceSize += snprintf(
            achSentence + n32SentenceSize,
            MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
            ",Alacarte");

        // Write to boundary stationary vehicle stuff.
        if(TRUE == psDenm->denm.alacarte.stationaryVehicle_option) {

            // Write to boundary vehicle identification stuff.
            if(TRUE == psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification_option) {

                if(TRUE == psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS_option) {

                    n32SentenceSize += snprintf(
                                achSentence + n32SentenceSize,
                                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                                ",VDS,%s",
                                psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS.buf);
                }

                if(TRUE == psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber_option) {

                    n32SentenceSize += snprintf(
                                achSentence + n32SentenceSize,
                                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                                ",wMInumber,%s",
                                psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber.buf);
                }
            }
        }

        if(TRUE == psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods_option) {

            n32SentenceSize += snprintf(
                        achSentence + n32SentenceSize,
                        MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                        ",IsCarryDangerous,true,Type,%d,CompanyName,%s",
                        psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.dangerousGoodsType,
                        psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName.buf);

        } else {

            n32SentenceSize += snprintf(
                        achSentence + n32SentenceSize,
                        MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                        ",IsCarryDangerous,false");
        }
    }

    n32SentenceSize += snprintf(
                achSentence + n32SentenceSize,
                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                ",Lat,%d,Lon,%d\n",
                psDenm->denm.management.eventPosition.latitude,
                psDenm->denm.management.eventPosition.longitude);

    g_fp_write_to_boundary(achSentence, n32SentenceSize, psDenm->header.stationID);
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

static int32_t denm_mngr_msg_init(SITSStationInfo *psStationInfo, DENM *psOutputDenm) {

    // Set header info.

    /* For the present document, the value of the DE protocolVersion shall be set to 1.  */
    psOutputDenm->header.protocolVersion = DENM_PROTOCOL_VERSION;
    psOutputDenm->header.messageID = DENM_Id;
    psOutputDenm->header.stationID = psStationInfo->m_un32StationId;

    /* Identifier generated by the DEN basic service for new DENM. */
    psOutputDenm->denm.management.actionID.originatingStationID = psStationInfo->m_un32StationId; /* StationID (0..4294967295) */
    psOutputDenm->denm.management.actionID.sequenceNumber = DENM_ACTION_SEQUENCE(); /* SequenceNumber (0..65535) */

    /* This DE provides the station type information of the originating ITS-S. */
    psOutputDenm->denm.management.stationType = psStationInfo->m_n32StationType;

    return PROCEDURE_SUCCESSFULL;
}

int32_t denm_mngr_msg_encode(uint8_t **p2un8DenmPayload, fix_data_t *psPotiFixData, DENM *psOutputDenm, ITSMsgCodecErr *psOutputErr) {

    int32_t n32EncodedBufferLength = 0;

    // ---------------------------------------------------------
    // --------------- Set Management Options ------------------
    // ---------------------------------------------------------

    /*
     * Time at which the event is detected by the originating ITS-S. For the DENM repetition, this DE shall remain unchanged.
     * For the DENM update, this DE shall be the time at which the event update is detected.
     * For the DENM termination, this DE shall be the time at which the termination of the event is detected.
     *
     * Number of milliseconds since 2004-01-01T00:00:00.000Z, as specified in ISO 8601 [i.10].
     */
    asn1_new_integer_u64(&psOutputDenm->denm.management.detectionTime, (uint64_t)DENM_EVENT_GET_POSITION_TIME());

    /*
     * This DE refers to the time at which a new DENM, an update DENM or a cancellation DENM is generated.
     *
     * Number of milliseconds since 2004-01-01T00:00:00.000Z, as specified in ISO 8601 [i.10].
     */
    asn1_new_integer_u64(&psOutputDenm->denm.management.referenceTime, (uint64_t)fmod(psPotiFixData->time.tai_since_2004 * 1000.0, 65536));

//    /* This DE indicates if the type of generated DENM is a cancellation DENM or a negation DENM. */
    psOutputDenm->denm.management.termination_option = FALSE; /* Send DENM_trigger for demonstration, no need to enable termination field. */
//    //psOutputDenm->denm.management.termination =

//    /* The distance within which the event is considered relevant to the receiving ITS-S. */
    psOutputDenm->denm.management.relevanceDistance_option = FALSE;
//    //psOutputDenm->denm.management.relevanceDistance =

//    /* The traffic direction along which the event information is relevant for the receiving ITS-S. */
    psOutputDenm->denm.management.relevanceTrafficDirection_option = FALSE;
//    //psOutputDenm->denm.management.relevanceTrafficDirection =

//    /* Validity duration of a DENM. */
    psOutputDenm->denm.management.validityDuration = DENM_VALIDITY_DURATION_DEF; /* ValidityDuration (0..86400) */

//    /* Time interval for DENM transmission as defined by the originating ITS-S. */
    psOutputDenm->denm.management.transmissionInterval_option = FALSE;
//    //psOutputDenm->denm.management.transmissionInterval = /* TransmissionInterval (1..10000) */

//    // ---------------------------------------------------------
//    // ---------------- Set Situation Options ------------------
//    // ---------------------------------------------------------

//    /*
//     *  The situation container includes information that describes the detected event.
//     */
//    psOutputDenm->denm.situation_option = TRUE; // TRUE; TODO DEBUG
//    psOutputDenm->denm.situation.informationQuality = 0; /* InformationQuality (0..7), If the information is unknown, the DE shall be set to 0. */
//    psOutputDenm->denm.situation.eventType.causeCode = 0; //CauseCodeType_redLight; //TODO DEUBG
//    psOutputDenm->denm.situation.eventType.subCauseCode = 0; // WrongWayDrivingSubCauseCode_unavailable; // TODO DEBUG

//    psOutputDenm->denm.situation.linkedCause_option = FALSE;
//    //psOutputDenm->denm.situation.linkedCause =
//    psOutputDenm->denm.situation.eventHistory_option = FALSE;
//    //psOutputDenm->denm.situation.eventHistory =

//    // ---------------------------------------------------------
//    // ---------------- Set Location Options -------------------
//    // ---------------------------------------------------------

//    /*
//     * The location container describes the location of the detected event.
//     */
//    psOutputDenm->denm.location_option = TRUE;
//    psOutputDenm->denm.location.eventSpeed_option = TRUE;
//    psOutputDenm->denm.location.eventSpeed.speedValue = SpeedValue_unavailable; /* 0,01 m/s */
//    psOutputDenm->denm.location.eventSpeed.speedConfidence = SpeedConfidence_equalOrWithinOneMeterPerSec;
//    psOutputDenm->denm.location.eventPositionHeading_option = FALSE;
//    //psOutputDenm->denm.location.eventPositionHeading =

//    /* Allocate and initialize each entries and paths. */
//    psOutputDenm->denm.location.traces.count = 1;
//    psOutputDenm->denm.location.traces.tab = (PathHistory_ITS *)calloc(sizeof(PathHistory_ITS), psOutputDenm->denm.location.traces.count);
//    psOutputDenm->denm.location.traces.tab[0].count = 2;
//    psOutputDenm->denm.location.traces.tab[0].tab = (PathPoint *)calloc(sizeof(PathPoint), psOutputDenm->denm.location.traces.tab[0].count);
//    psOutputDenm->denm.location.traces.tab[0].tab[0].pathPosition.deltaLatitude = -123010;
//    psOutputDenm->denm.location.traces.tab[0].tab[0].pathPosition.deltaLongitude = -131068;
//    psOutputDenm->denm.location.traces.tab[0].tab[0].pathPosition.deltaAltitude = -12693;
//    psOutputDenm->denm.location.traces.tab[0].tab[1].pathPosition.deltaLatitude = -131058;
//    psOutputDenm->denm.location.traces.tab[0].tab[1].pathPosition.deltaLongitude = -131055;
//    psOutputDenm->denm.location.traces.tab[0].tab[1].pathPosition.deltaAltitude = -11769;
//    psOutputDenm->denm.location.traces.tab[0].tab[1].pathDeltaTime_option = TRUE;
//    psOutputDenm->denm.location.traces.tab[0].tab[1].pathDeltaTime = -151310;

//    psOutputDenm->denm.location.roadType_option = FALSE;
//    //psOutputDenm->denm.location.roadType

//    // ---------------------------------------------------------
//    // ---------------- Set ALACARTE Options -------------------
//    // ---------------------------------------------------------

//    /*
//     * The a la carte container contains additional information that is not provided by other containers.
//     */
//    psOutputDenm->denm.alacarte_option = TRUE;
//    psOutputDenm->denm.alacarte.stationaryVehicle_option = TRUE;
//    psOutputDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods_option = TRUE;
//    psOutputDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName_option = TRUE;
//    psOutputDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName.buf = (uint8_t*)g_sScenarioInfo.m_achParticipantName;
//    psOutputDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName.len = strlen(g_sScenarioInfo.m_achParticipantName) + 1;
//    /*
//    psOutputDenm->denm.alacarte_option = TRUE;
//    psOutputDenm->denm.alacarte.impactReduction_option = TRUE;
//    psOutputDenm->denm.alacarte.impactReduction.heightLonCarrLeft = 1;
//    psOutputDenm->denm.alacarte.impactReduction.heightLonCarrRight = 1;
//    psOutputDenm->denm.alacarte.impactReduction.posLonCarrLeft = 1;
//    psOutputDenm->denm.alacarte.impactReduction.posLonCarrRight = 1;
//    psOutputDenm->denm.alacarte.impactReduction.posCentMass = 1;
//    psOutputDenm->denm.alacarte.impactReduction.wheelBaseVehicle = 1;
//    psOutputDenm->denm.alacarte.impactReduction.turningRadius = 1;
//    psOutputDenm->denm.alacarte.impactReduction.posFrontAx = 1;
//    psOutputDenm->denm.alacarte.impactReduction.positionOfOccupants. = 20;
//    psOutputDenm->denm.alacarte.impactReduction.vehicleMass = 16;
//    */
//    //psOutputDenm->denm.alacarte.lanePosition_option =
//    //psOutputDenm->denm.alacarte.lanePosition =
//    //psOutputDenm->denm.alacarte.impactReduction_option =
//    //psOutputDenm->denm.alacarte.externalTemperature_option =
//    //psOutputDenm->denm.alacarte.externalTemperature =
//    //psOutputDenm->denm.alacarte.roadWorks_option =
//    //psOutputDenm->denm.alacarte.roadWorks =
//    //psOutputDenm->denm.alacarte.positioningSolution_option =
//    //psOutputDenm->denm.alacarte.positioningSolution =
//    //psOutputDenm->denm.alacarte.stationaryVehicle_option =
//    //psOutputDenm->denm.alacarte.stationaryVehicle =

    // ---------------------------------------------------------
    // ----------------- Encode ITS Message --------------------
    // ---------------------------------------------------------

    n32EncodedBufferLength = itsmsg_encode(p2un8DenmPayload, (ItsPduHeader *)psOutputDenm, psOutputErr);

    // Release allocated memory.
//    free(psOutputDenm->denm.location.traces.tab[0].tab);
//    free(psOutputDenm->denm.location.traces.tab);
    asn1_free_integer(&psOutputDenm->denm.management.detectionTime);
    asn1_free_integer(&psOutputDenm->denm.management.referenceTime);

    return n32EncodedBufferLength;
}

int32_t denm_mngr_msg_decode(uint8_t *pun8RxPayload, int32_t n32RxPayloadLength, btp_handler_recv_indicator_t *psBtpRecvStatus, bool bSspCheck, DENM *psOutputDenm, ITSMsgCodecErr *psOutputErr) {

    ItsPduHeader *psItsHeader = NULL;
    uint64_t un64DetectionTime;
    uint64_t un64ReferenceTime;
    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    if(psBtpRecvStatus == NULL
            || pun8RxPayload == NULL
            || n32RxPayloadLength == 0
            || NULL == psOutputDenm
            || NULL == psOutputErr) {

        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    /* Check whether this is a ITS message. */
    if(0 < (itsmsg_decode(&psItsHeader, pun8RxPayload, n32RxPayloadLength, psOutputErr))) {

        /* Check whether this is a ITS CAM message. */
        if(DENM_Id == psItsHeader->messageID) {

            if(TRUE == bSspCheck) {

                /* Check CAM msg permission */
                n32Result = denm_mngr_check_msg_permissions(
                            psOutputDenm,
                            psBtpRecvStatus->security.ssp,
                            psBtpRecvStatus->security.ssp_len);

                if(FALSE == IS_SUCCESS(n32Result)) {

                    printf("Received DENM message is untrustworthy\n");
                    n32Result = PROCEDURE_SECURITY_ERROR;

                } else {

                    memcpy(psOutputDenm, psItsHeader, sizeof(DENM));

                    asn1_get_integer_si64_ov(&psOutputDenm->denm.management.detectionTime, (int64_t *)&un64DetectionTime);
                    asn1_get_integer_si64_ov(&psOutputDenm->denm.management.referenceTime, (int64_t *)&un64ReferenceTime);
                }
            }

        } else {

            printf("Received unrecognized ITS message type: %d\n", psItsHeader->messageID);
        }
    }

    return n32Result;
}

static int32_t denm_mngr_check_msg_permissions(DENM *psOutputDenm, uint8_t *pun8Ssp, uint32_t un32SspLength) {

    int32_t rc = 0;
    if (un32SspLength < DENM_SSP_LEN) {
        rc = -1;
        printf("Err: SSP length[%d] is not enough\n", un32SspLength);
        goto FAILURE;
    }

    if (psOutputDenm->denm.situation_option) {
        /*
         *   For example, only check wrongWayDriving
         *   Please refer to ETSI EN 302 637-3 to check related SSP item
         */
        switch (psOutputDenm->denm.situation.eventType.causeCode) {
            case CauseCodeType_wrongWayDriving:
                if (DENM_SSP_CHECK(WRONG_WAY_DRIVING, pun8Ssp[2]) == false) {
                    printf("Err: certificate not allowed to sign WRONG_WAY_DRIVING\n");
                    rc = -1;
                    goto FAILURE;
                }
                break;
            default:
                // nothing
                break;
        }
    }

FAILURE:
    return rc;
}
