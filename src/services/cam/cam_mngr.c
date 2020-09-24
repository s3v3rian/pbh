#include "cam_mngr.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/inc/error_code_user.h"

#include "common/utils/geo_utils.h"

#include "services/gps/nmea_infra.h"

/*
 *******************************************************************************
 * Private function signatures
 *******************************************************************************
 */

static int32_t cam_mngr_msg_init(SStationInfo *psStationInfo, CAM *psOutputCam);
static int32_t cam_mngr_msg_encode(uint8_t **p2un8CamPayload, fix_data_t *psPotiFixData, CAM *psOutputCam, ITSMsgCodecErr *psOutputErr);
static int32_t cam_mngr_msg_decode(uint8_t *pun8RxPayload, int32_t n32RxPayloadLength, btp_handler_recv_indicator_t *psBtpRecvStatus, bool bSspCheck, CAM *psOutputCam, ITSMsgCodecErr *psOutputErr);

static int32_t cam_mngr_set_semi_axis_length(double dMeter);
static int32_t cam_mngr_set_altitude_confidence(double dMeter);
static int32_t cam_mngr_set_heading_confidence(double dDegree);
static int32_t cam_mngr_set_speed_confidence(double dMeterPerSec);
static int32_t cam_mngr_check_msg_permissions(CAM *psOutputCam, uint8_t *pun8Ssp, uint32_t un32SspLength);

/*
 *******************************************************************************
 * Private/Extern variables
 *******************************************************************************
 */

// BTP fields.
btp_handler_ptr m_pBtpCamHandler;
btp_handler_send_config_t m_sBtpCamSendConfig;

// CAM generation fields.
ITSMsgCodecErr m_sEncodeCamErr;

// CAM decode fields.
uint8_t m_aun8CamRxPayload[GN_MAX_SDU_SIZE];
ITSMsgCodecErr m_sDecodeCamErr;
btp_handler_recv_indicator_t m_sBtpCamRecvStatus;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t cam_mngr_init() {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    uint16_t un16CamSrcPrt = CAM_BTP_PORT; /* Listen CAM message from remote ITS station. */
    uint16_t un16CamDestPrt = CAM_BTP_PORT; /* Broadcast CAM message to remote ITS station. */

    btp_handler_config_t sBtpCamConfig;

    BTP_HDLR_CONFIG_INIT(&sBtpCamConfig, un16CamDestPrt);

#if (__EN_SECURITY_FEATURE__)

    /*
     * Enable AID security mode.
     * Please make sure all the security configuration in gnd.json is set correctly.
     */
    sBtpCamConfig.security.type = GN_SEC_ENCAP_PKT_BY_AID_PROFILE;
    sBtpCamConfig.security.its_aid = ITS_SEC_CAM_AID;

#endif

    // Create BTP handler.
    n32Result = btp_create(&m_pBtpCamHandler, &sBtpCamConfig);

    if(false == IS_SUCCESS(n32Result)) {

        printf("Cannot create BTP handler: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    /* Bind BTP port. */
    n32Result = btp_bind(m_pBtpCamHandler, un16CamSrcPrt);

    if(false == IS_SUCCESS(n32Result)) {

        printf("Cannot bind BTP handler: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    /* BTP send packet default configuration (SHB/BTP-B). */
    BTP_SEND_CONFIG_INIT(&m_sBtpCamSendConfig);

#if (__EN_SECURITY_FEATURE__)

    /*
    * Assign CAM service specific permissions according to the actual content in payload.
    * Please refer to ETSI TS 103 097, ETSI EN 302 637-2 for more details.
    * Please refer to Unex-APG-ETSI-GN-BTP for more information of build-in certificates
    */

    /* SSP Version control */
    m_sBtpCamSendConfig.security.ssp[0] = 0x0;
    /* Service-specific parameter */
    m_sBtpCamSendConfig.security.ssp[1] = EMERGENCY; /* Emergency container */
    m_sBtpCamSendConfig.security.ssp[2] = REQUEST_FOR_FREE_CROSSING_AT_A_TRAFFIC_LIGHT; /* EmergencyPriority */
    m_sBtpCamSendConfig.security.ssp_len = 3;

#endif

    /* Allocate a buffer for restoring the decode error information if needed. */
    m_sDecodeCamErr.msg_size = 256;
    m_sDecodeCamErr.msg = calloc(1, m_sDecodeCamErr.msg_size);

    /* Allocate a buffer for restoring the decode error information if needed. */
    m_sEncodeCamErr.msg_size = 512;
    m_sEncodeCamErr.msg = calloc(1, m_sEncodeCamErr.msg_size);

    if(NULL == m_sEncodeCamErr.msg
            || NULL == m_sDecodeCamErr.msg) {

        printf("Cannot allocate memory for CAM error message buffer.\n");

        n32Result = PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    return n32Result;
}

int32_t cam_mngr_process_tx(SStationInfo *psStationInfo, fix_data_t *psPotiFixData, CAM *psOutputCam) {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    uint8_t *pun8TxPayload = NULL;
    int32_t n32TxPayloadSize = 0;

    //printf("Preparing CAM message\n");

    cam_mngr_msg_init(psStationInfo, psOutputCam);

    // Generate CAM message.
    n32TxPayloadSize = cam_mngr_msg_encode(
                &pun8TxPayload,
                psPotiFixData,
                psOutputCam,
                &m_sEncodeCamErr);

    if(pun8TxPayload == NULL
            || n32TxPayloadSize <= 0) {

        printf("Failed to generate CAM message\n");
        return PROCEDURE_INVALID_SERVICE_TX_ERROR;
    }

    printf("Sending CAM message\n");

    /* Broadcast the CAM message (BTP-B, SHB). */
    n32Result = btp_send(m_pBtpCamHandler, &m_sBtpCamSendConfig, pun8TxPayload, n32TxPayloadSize);

    if(0 >= n32Result) {

        printf("Transmitting error: %s\n", ERROR_MSG(n32Result));
    }

    // Release the allocated message buffer.
    itsmsg_buf_free(pun8TxPayload);

    return n32Result;
}

int32_t cam_mngr_process_rx(CAM *psOutputCam) {

    bool bSspCheck = false;

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    /* Listen the CAM BTP port. */
    int32_t n32CamDataSize = btp_recv(
                m_pBtpCamHandler,
                &m_sBtpCamRecvStatus,
                m_aun8CamRxPayload,
                sizeof(m_aun8CamRxPayload),
                BTP_RECV_WAIT_FOREVER);

    if(0 > n32CamDataSize > 0) {

        printf("Received from CAM port something stinky... Error: %d", n32Result);
        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    printf("Processing received CAM\n");

    if(true == IS_DECAP_FAIL(m_sBtpCamRecvStatus.security.status)) {

        printf("\tSecurity status: decapsulation error (%d), the payload content is untrustworthy\n", m_sBtpCamRecvStatus.security.status);
        return PROCEDURE_SECURITY_ERROR;
    }

    if(ITS_SEC_SUCCESS == m_sBtpCamRecvStatus.security.status
            || ITS_SEC_NA == m_sBtpCamRecvStatus.security.status) {

        if(ITS_SEC_SUCCESS == m_sBtpCamRecvStatus.security.status) {

            bSspCheck = true;
        }

        /* Try to decode the received message. */
        n32Result = cam_mngr_msg_decode(
                    m_aun8CamRxPayload,
                    n32CamDataSize,
                    &m_sBtpCamRecvStatus,
                    bSspCheck,
                    psOutputCam,
                    &m_sDecodeCamErr);

    } else if(m_sBtpCamRecvStatus.security.status == ITS_SEC_FAIL) {

        printf("\tSecurity status failure (%d)\n", m_sBtpCamRecvStatus.security.status);
    }

    if(0 > n32Result) {

        printf("Unable to decode CAM RX packet: %s\n", m_sDecodeCamErr.msg);
    }

    return n32Result;
}

int32_t cam_mngr_release() {

    btp_release(m_pBtpCamHandler);

    free(m_sDecodeCamErr.msg);

    return PROCEDURE_SUCCESSFULL;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

static int32_t cam_mngr_msg_init(SStationInfo *psStationInfo, CAM *psOutputCam) {

    // Set header info.

    /* For the present document, the value of the DE protocolVersion shall be set to 1.  */
    psOutputCam->header.protocolVersion = CAM_PROTOCOL_VERSION;
    psOutputCam->header.messageID = CAM_Id;

    if(0 == psOutputCam->header.stationID) {

        psOutputCam->header.stationID = psStationInfo->m_un32StationId;
    }

    if(0 == psOutputCam->cam.camParameters.basicContainer.stationType) {

        /* This DE provides the station type information of the originating ITS-S. */
        psOutputCam->cam.camParameters.basicContainer.stationType = (15 < psStationInfo->m_n32StationType) ? 15 : psStationInfo->m_n32StationType;
        //psOutputCam->cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.vehicleRole = psStationInfo->m_n32SubStationType; // TODO
    }

    return PROCEDURE_SUCCESSFULL;
}

static int32_t cam_mngr_msg_encode(uint8_t **p2un8CamPayload, fix_data_t *psPotiFixData, CAM *psOutputCam, ITSMsgCodecErr *psOutputErr) {

    int buf_len = 0;

    /*
     * Time corresponding to the time of the reference position in the CAM, considered
     * as time of the CAM generation.
     * The value of the DE shall be wrapped to 65 536. This value shall be set as the
     * remainder of the corresponding value of TimestampIts divided by 65 536 as below:
     *      generationDeltaTime = TimestampIts mod 65 536
     * TimestampIts represents an integer value in milliseconds since 2004-01-01T00:00:00:000Z
     * as defined in ETSI TS 102 894-2 [2].
     */
    psOutputCam->cam.generationDeltaTime = (int32_t)fmod(psPotiFixData->time.tai_since_2004 * 1000.0, 65536); /* TAI milliseconds since 2004-01-01 00:00:00.000 UTC. */

    /*
     * Position and position accuracy measured at the reference point of the originating
     * ITS-S. The measurement time shall correspond to generationDeltaTime.
     * If the station type of the originating ITS-S is set to one out of the values 3 to 11
     * the reference point shall be the ground position of the centre of the front side of
     * the bounding box of the vehicle.
     * The positionConfidenceEllipse provides the accuracy of the measured position
     * with the 95 % confidence level. Otherwise, the positionConfidenceEllipse shall be
     * set to unavailable.
     * If semiMajorOrientation is set to 0 degree North, then the semiMajorConfidence
     * corresponds to the position accuracy in the North/South direction, while the
     * semiMinorConfidence corresponds to the position accuracy in the East/West
     * direction. This definition implies that the semiMajorConfidence might be smaller
     * than the semiMinorConfidence.
     */
    if(0 == psOutputCam->cam.camParameters.basicContainer.referencePosition.latitude
            || 0 == psOutputCam->cam.camParameters.basicContainer.referencePosition.longitude) {

            psOutputCam->cam.camParameters.basicContainer.referencePosition.latitude = (int32_t)(psPotiFixData->latitude * 10000000.0); /* Convert to 1/10 micro degree. */
            psOutputCam->cam.camParameters.basicContainer.referencePosition.longitude = (int32_t)(psPotiFixData->longitude * 10000000.0); /* Convert to 1/10 micro degree. */
            psOutputCam->cam.camParameters.basicContainer.referencePosition.positionConfidenceEllipse.semiMajorConfidence = cam_mngr_set_semi_axis_length(psPotiFixData->err_smajor_axis); /* Convert to centimetre. */
            psOutputCam->cam.camParameters.basicContainer.referencePosition.positionConfidenceEllipse.semiMinorConfidence = cam_mngr_set_semi_axis_length(psPotiFixData->err_sminor_axis); /* Convert to centimetre. */
            psOutputCam->cam.camParameters.basicContainer.referencePosition.positionConfidenceEllipse.semiMajorOrientation = (int32_t)(psPotiFixData->err_smajor_orientation * 10.0); /* Convert to 0.1 degree from North. */
            psOutputCam->cam.camParameters.basicContainer.referencePosition.altitude.altitudeValue = (int32_t)(psPotiFixData->altitude * 100.0); /* Convert to 0.01 metre. */
            psOutputCam->cam.camParameters.basicContainer.referencePosition.altitude.altitudeConfidence = cam_mngr_set_altitude_confidence(psPotiFixData->err_altitude);
    }

    /*
     * The mandatory high frequency container of CAM.
     */

    if(GN_ITS_STATION_ROAD_SIDE_UNIT != g_sLocalStationInfo.m_n32StationType) { // TODO DEBUG
    /* Heading. */
    //psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.heading.headingValue = (int32_t)(psPotiFixData->course_over_ground * 10.0); /* Convert to 0.1 degree from North. */
    //psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.heading.headingConfidence = cam_mngr_set_heading_confidence(psPotiFixData->err_course_over_ground); /* Convert to 1 ~ 127 enumeration. */

    /* Speed, 0.01 m/s */
    //psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedValue = (int16_t)(psPotiFixData->horizontal_speed * 100.0); /* Convert to 0.01 metre per second. */
    //psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedConfidence = cam_mngr_set_speed_confidence(psPotiFixData->err_horizontal_speed);

    /* Direction. */
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.driveDirection = CAM_SENSOR_GET_DRIVE_DIRECTION();

    /* Vehicle length, 0.1 metre  */
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue = CAM_SENSOR_GET_VEHICLE_LENGTH_VALUE();
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthConfidenceIndication = CAM_SENSOR_GET_VEHICLE_LENGTH_CONF();

    /* Vehicle width, 0.1 metre */
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleWidth = CAM_SENSOR_GET_VEGICLE_WIDTH_VALUE();

    /* Longitudinal acceleration, 0.1 m/s^2 */
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.longitudinalAcceleration.longitudinalAccelerationValue = CAM_SENSOR_GET_LONG_ACCEL_VALUE();
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.longitudinalAcceleration.longitudinalAccelerationConfidence = CAM_SENSOR_GET_LONG_ACCEL_CONF();

    /*
     * Curvature value, 1 over 30000 meters, (-30000 .. 30001)
     * The confidence value shall be set to:
     *      0 if the accuracy is less than or equal to 0,00002 m-1
     *      1 if the accuracy is less than or equal to 0,0001 m-1
     *      2 if the accuracy is less than or equal to 0,0005 m-1
     *      3 if the accuracy is less than or equal to 0,002 m-1
     *      4 if the accuracy is less than or equal to 0,01 m-1
     *      5 if the accuracy is less than or equal to 0,1 m-1
     *      6 if the accuracy is out of range, i.e. greater than 0,1 m-1
     *      7 if the information is not available
     */
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.curvature.curvatureValue = CAM_SENSOR_GET_CURVATURE_VALUE();
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.curvature.curvatureConfidence = CAM_SENSOR_GET_CURVATURE_CONF();
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.curvatureCalculationMode = CAM_SENSOR_GET_CURVATURE_CONF_CAL_MODE();
    /* YAW rate, 0,01 degree per second. */
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.yawRate.yawRateValue = CAM_SENSOR_GET_YAW_RATE_VALUE();
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.yawRate.yawRateConfidence = CAM_SENSOR_GET_YAW_RATE_CONF();
    /* Optional fields, disable all by default. */
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.accelerationControl_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.accelerationControl =
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.lanePosition_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.lanePosition =
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.steeringWheelAngle_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.steeringWheelAngle =
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.lateralAcceleration_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.lateralAcceleration =
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.verticalAcceleration_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.verticalAcceleration =
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.performanceClass_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.performanceClass =
    psOutputCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.cenDsrcTollingZone_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.cenDsrcTollingZone =
}
    /*
    *   If stationType is set to specialVehicles(10)
    *       LowFrequencyContainer shall be set to BasicVehicleContainerLowFrequency
    *       SpecialVehicleContainer shall be set to EmergencyContainer
    */
    if(psOutputCam->cam.camParameters.basicContainer.stationType == GN_ITS_STATION_SPECIAL_VEHICLE) {
        /*
        * The optional low frequency container of CAM.
        *      vehicleRole: emergency(6)
        *      exteriorLights: select highBeamHeadlightsOn (1)
        *           lowBeamHeadlightsOn (0),
        *           highBeamHeadlightsOn (1),
        *           leftTurnSignalOn (2),
        *           rightTurnSignalOn (3),
        *           daytimeRunningLightsOn (4),
        *           reverseLightOn (5),
        *           fogLightOn (6),
        *           parkingLightsOn (7)
        *      pathHistory: set zero historical path points
        */
        //psOutputCam->cam.camParameters.lowFrequencyContainer_option = true;
        //psOutputCam->cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.vehicleRole = VehicleRole_emergency;
        /* alloc and set the bit of bitstring */
        //asn1_bstr_alloc_set_bit(&(psOutputCam->cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.exteriorLights), ExteriorLights_MAX_BITS_ITS, ExteriorLights_highBeamHeadlightsOn_ITS);
        //psOutputCam->cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.pathHistory.count = 0;

        /*
        * The optional special vehicle container of CAM.
        *       lightBarSirenInUse: select sirenActivated (1)
        *           lightBarActivated (0)
        *           sirenActivated (1)
        *       emergencyPriority: enable , select requestForFreeCrossingAtATrafficLight (1)
        *           requestForRightOfWay (0)
        *           requestForFreeCrossingAtATrafficLight (1)
        *       causeCode/subCauseCode: disable
        */
        //psOutputCam->cam.camParameters.specialVehicleContainer_option = true;
        //psOutputCam->cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_emergencyContainer;
        /* alloc and set the bit of bitstring */
        //asn1_bstr_alloc_set_bit(&(psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.lightBarSirenInUse), LightBarSirenInUse_MAX_BITS, LightBarSirenInUse_sirenActivated);
        //psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority_option = true;
        //asn1_bstr_alloc_set_bit(&(psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority), EmergencyPriority_MAX_BITS, EmergencyPriority_requestForFreeCrossingAtATrafficLight);
        //psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication_option = false;
    }

    if(TRUE == psOutputCam->cam.camParameters.specialVehicleContainer_option) {

        asn1_bstr_alloc_set_bit(&(psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.lightBarSirenInUse), LightBarSirenInUse_MAX_BITS, LightBarSirenInUse_sirenActivated);
    }

    /* Encode the CAM message. */
    buf_len = itsmsg_encode(p2un8CamPayload, (ItsPduHeader *)psOutputCam, psOutputErr);

    if(buf_len <= 0) {
        printf("itsmsg_encode error: %s\n", psOutputErr->msg);
    }

    if(GN_ITS_STATION_ROAD_SIDE_UNIT == psOutputCam->cam.camParameters.basicContainer.stationType) {

        asn1_bstr_free(&(psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.lightBarSirenInUse));

    } else if(psOutputCam->cam.camParameters.basicContainer.stationType == GN_ITS_STATION_SPECIAL_VEHICLE) {

        // free the memory for encoding/
        asn1_bstr_free(&(psOutputCam->cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.exteriorLights));
        asn1_bstr_free(&(psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.lightBarSirenInUse));
        asn1_bstr_free(&(psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority));
    }

    return buf_len;
}

static int32_t cam_mngr_msg_decode(uint8_t *pun8RxPayload, int32_t n32RxPayloadLength, btp_handler_recv_indicator_t *psBtpRecvStatus, bool bSspCheck, CAM *psOutputCam, ITSMsgCodecErr *psOutputErr) {

    ItsPduHeader *psItsHeader = NULL;
    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    if(psBtpRecvStatus == NULL
            || pun8RxPayload == NULL
            || n32RxPayloadLength == 0
            || NULL == psOutputCam
            || NULL == psOutputErr) {

        return -1;
    }

    /* Check whether this is a ITS message. */
    if(0 < (itsmsg_decode(&psItsHeader, pun8RxPayload, n32RxPayloadLength, psOutputErr))) {

        /* Check whether this is a ITS CAM message. */
        if(psItsHeader->messageID == CAM_Id) {

            memcpy(psOutputCam, psItsHeader, sizeof(CAM));
            n32Result = PROCEDURE_SUCCESSFULL;
            /* TODO DEBUG
            if(TRUE == bSspCheck) {

                n32Result = cam_mngr_check_msg_permissions(psOutputCam, psBtpRecvStatus->security.ssp, psBtpRecvStatus->security.ssp_len);
                if(FALSE == IS_SUCCESS(n32Result)) {

                    printf("Received CAM message is untrustworthy\n");
                    n32Result = PROCEDURE_SECURITY_ERROR;

                } else {

                    memcpy(psOutputCam, psItsHeader, sizeof(CAM));
                    n32Result = PROCEDURE_SUCCESSFULL;
                }
            }
            */

        } else {

            printf("Received unrecognized ITS message type: %d\n", psItsHeader->messageID);
            n32Result = PROCEDURE_INVALID_SERVICE_RX_ERROR;
        }
    }

    return n32Result;
}

static int32_t cam_mngr_set_semi_axis_length(double dMeter) {

    /*
     * According to ETSI TS 102 894-2 V1.2.1 (2014-09)
     * The value shall be set to:
     * 1 if the accuracy is equal to or less than 1 cm,
     * n (n > 1 and n < 4 093) if the accuracy is equal to or less than n cm,
     * 4 093 if the accuracy is equal to or less than 4 093 cm,
     * 4 094 if the accuracy is out of range, i.e. greater than 4 093 cm,
     * 4 095 if the accuracy information is unavailable.
     */
    double dCentimeter;
    int32_t n32Value;

    dCentimeter = dMeter * 100.0;

    if(dCentimeter < 1.0) {
        n32Value = 1;
    }
    else if(dCentimeter > 1.0 && dCentimeter < 4093.0) {
        n32Value = (int32_t)dCentimeter;
    }
    else {
        n32Value = 4094;
    }

    return n32Value;
}

static int32_t cam_mngr_set_altitude_confidence(double dMeter)
{
    /*
     * According to ETSI TS 102 894-2 V1.2.1 (2014-09)
     * Absolute accuracy of a reported altitude value of a geographical point for a predefined
     * confidence level (e.g. 95 %). The required confidence level is defined by the
     * corresponding standards applying the usage of this DE.
     * The value shall be set to:
     * 	0 if the altitude accuracy is equal to or less than 0,01 metre
     * 	1 if the altitude accuracy is equal to or less than 0,02 metre
     * 	2 if the altitude accuracy is equal to or less than 0,05 metre
     * 	3 if the altitude accuracy is equal to or less than 0,1 metre
     * 	4 if the altitude accuracy is equal to or less than 0,2 metre
     * 	5 if the altitude accuracy is equal to or less than 0,5 metre
     * 	6 if the altitude accuracy is equal to or less than 1 metre
     * 	7 if the altitude accuracy is equal to or less than 2 metres
     * 	8 if the altitude accuracy is equal to or less than 5 metres
     * 	9 if the altitude accuracy is equal to or less than 10 metres
     * 	10 if the altitude accuracy is equal to or less than 20 metres
     * 	11 if the altitude accuracy is equal to or less than 50 metres
     * 	12 if the altitude accuracy is equal to or less than 100 metres
     * 	13 if the altitude accuracy is equal to or less than 200 metres
     * 	14 if the altitude accuracy is out of range, i.e. greater than 200 metres
     * 	15 if the altitude accuracy information is unavailable
     */

    int32_t n32EnumValue;

    if(dMeter <= 0.01) {
        n32EnumValue = AltitudeConfidence_alt_000_01;
    }
    else if(dMeter <= 0.02) {
        n32EnumValue = AltitudeConfidence_alt_000_02;
    }
    else if(dMeter <= 0.05) {
        n32EnumValue = AltitudeConfidence_alt_000_05;
    }
    else if(dMeter <= 0.1) {
        n32EnumValue = AltitudeConfidence_alt_000_10;
    }
    else if(dMeter <= 0.2) {
        n32EnumValue = AltitudeConfidence_alt_000_20;
    }
    else if(dMeter <= 0.5) {
        n32EnumValue = AltitudeConfidence_alt_000_50;
    }
    else if(dMeter <= 1.0) {
        n32EnumValue = AltitudeConfidence_alt_001_00;
    }
    else if(dMeter <= 2.0) {
        n32EnumValue = AltitudeConfidence_alt_002_00;
    }
    else if(dMeter <= 5.0) {
        n32EnumValue = AltitudeConfidence_alt_005_00;
    }
    else if(dMeter <= 10.0) {
        n32EnumValue = AltitudeConfidence_alt_010_00;
    }
    else if(dMeter <= 20.0) {
        n32EnumValue = AltitudeConfidence_alt_020_00;
    }
    else if(dMeter <= 50.0) {
        n32EnumValue = AltitudeConfidence_alt_050_00;
    }
    else if(dMeter <= 100.0) {
        n32EnumValue = AltitudeConfidence_alt_100_00;
    }
    else if(dMeter <= 200.0) {
        n32EnumValue = AltitudeConfidence_alt_200_00;
    }
    else {
        n32EnumValue = AltitudeConfidence_outOfRange;
    }

    return n32EnumValue;
}

static int32_t cam_mngr_set_heading_confidence(double dDegree) {

    /*
     * According to ETSI TS 102 894-2 V1.2.1 (2014-09)
     *
     * The absolute accuracy of a reported heading value for a predefined confidence level
     * (e.g. 95 %). The required confidence level is defined by the corresponding standards
     * applying the DE.
     * The value shall be set to:
     * • 1 if the heading accuracy is equal to or less than 0,1 degree,
     * • n (n > 1 and n < 125) if the heading accuracy is equal to or less than
     * n × 0,1 degree,
     * 	125 if the heading accuracy is equal to or less than 12,5 degrees,
     * 	126 if the heading accuracy is out of range, i.e. greater than 12,5 degrees,
     * 	127 if the heading accuracy information is not available.
     */

    int32_t n32Value;

    if(dDegree <= 0.1) {
        n32Value = 1;
    }
    else if(dDegree > 1.0 && dDegree < 125.0) {
        n32Value = (int32_t)(dDegree * 0.1);
    }
    else {
        n32Value = 126;
    }

    return n32Value;
}

static int32_t cam_mngr_set_speed_confidence(double dMeterPerSec) {

    /*
     * According to ETSI TS 102 894-2 V1.2.1 (2014-09)
     * The value shall be set to:
     * 	1 if the speed accuracy is equal to or less than 1 cm/s.
     * 	n (n > 1 and n < 125) if the speed accuracy is equal to or less than n cm/s.
     * 	125 if the speed accuracy is equal to or less than 125 cm/s.
     * 	126 if the speed accuracy is out of range, i.e. greater than 125 cm/s.
     * 	127 if the speed accuracy information is not available.
     */

    double dCmPerSec;
    int32_t n32Value;

    dCmPerSec = dMeterPerSec * 100.0;

    if(dCmPerSec <= 1.0) {
        n32Value = 1;
    }
    else if(dCmPerSec > 1.0 && dCmPerSec < 125.0) {
        n32Value = (int32_t)(dCmPerSec);
    }
    else if(dCmPerSec >= 125.0 && dCmPerSec < 126.0) {
        n32Value = 125;
    }
    else {
        n32Value = 126;
    }

    return n32Value;
}

static int32_t cam_mngr_check_msg_permissions(CAM *psOutputCam, uint8_t *pun8Ssp, uint32_t un32SspLength) {

    int32_t n32Rc = 0;
    int32_t n32Fbs = 0;

    if(un32SspLength < CAM_SSP_LEN) {
        n32Rc = -1;
        printf("Err: SSP length[%d] is not enough\n", un32SspLength);
        goto FAILURE;
    }

    if(psOutputCam->cam.camParameters.specialVehicleContainer_option) {
        /*
         *   For example, only check emergencyContainer
         *   Please refer to ETSI EN 302 637-2 to check related SSP item
         */
        switch(psOutputCam->cam.camParameters.specialVehicleContainer.choice) {
            case SpecialVehicleContainer_emergencyContainer:
                if(CAM_SSP_CHECK(EMERGENCY, pun8Ssp[1]) == false) {
                    printf("Err: certificate not allowed to sign EMERGENCY\n");
                    n32Rc = -1;
                    goto FAILURE;
                }

                if(psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority_option) {
                    n32Fbs = asn1_bstr_ffs(&(psOutputCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority));
                    switch(n32Fbs) {
                        case EmergencyPriority_requestForRightOfWay:
                            if(CAM_SSP_CHECK(REQUEST_FOR_RIGHT_OF_WAY, pun8Ssp[2]) == false) {
                                printf("Err: certificate not allowed to sign REQUEST_FOR_RIGHT_OF_WAY\n");
                                n32Rc = -1;
                                goto FAILURE;
                            }
                            break;
                        case EmergencyPriority_requestForFreeCrossingAtATrafficLight:
                            if(CAM_SSP_CHECK(REQUEST_FOR_FREE_CROSSING_AT_A_TRAFFIC_LIGHT, pun8Ssp[2]) == false) {
                                printf("Err: certificate not allowed to sign REQUEST_FOR_FREE_CROSSING_AT_A_TRAFFIC_LIGHT\n");
                                n32Rc = -1;
                                goto FAILURE;
                            }
                            break;
                    }
                }
                break;
            default:
                // nothing
                break;
        }
    }

FAILURE:
    return n32Rc;
}
