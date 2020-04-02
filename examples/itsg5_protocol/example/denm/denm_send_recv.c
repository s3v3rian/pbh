/**
 *******************************************************************************
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * This example program is created for demonstrating how to perform DENM (Decentralized
 * Environmental Notification) service packet sending and receiving procedure which are
 * defined in ETSI EN 302 637-3 V1.2.2 (2014-11) Intelligent Transport Systems (ITS);
 * Vehicular Communications; Basic Set of Applications; Part 3: Specifications of
 * Decentralized Environmental Notification Basic Service
 *
 * Usage:
 *      (1) Correct the toolchain setting then type 'make' to build this example program.
 *      (2) Upload the 'denm_send_recv' execution file to 2 different Unex V2X modules.
 *      (3) Run the 'denm_send_recv' on module A and module B, then check whether these 2
 *          devices received DENM message from each other.
 *
 * @file    denm_send_recv.c
 * @brief
 * @author  Unex
 *******************************************************************************
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <inttypes.h>

#include "itsmsg.h"
#include "itsmsg_codec.h"
#include "poti_service.h"
#include "gn_service_user.h"
#include "error_code_user.h"


/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */
/* clang-format off */

#define DENM_BTP_PORT                               2002    /* ETSI TS 103 248. */

#define DENM_PROTOCOL_VERSION                       1       /* ETSI EN 302 637-3 V1.2.2 (2014-11) */

/* Please assign ID for your ITS station. */
#define DENM_STATION_ID_DEF                         168

/* Please assign the type of your ITS station. */
#define DENM_STATION_TYPE_DEF                       GN_ITS_STATION_ROAD_SIDE_UNIT

/* Please correct this setting according to different purpose, or keep this setting as default value. */
#define DENM_VALIDITY_DURATION_DEF                  600

/* Please assign sequence number for different action message. */
#define DENM_ACTION_SEQUENCE()                      0

/* Predefined event time and position for demonstration. */
#define DENM_EVENT_GET_POSITION_LAT()               (25.062498)
#define DENM_EVENT_GET_POSITION_LONG()              (170.842173)
#define DENM_EVENT_GET_POSITION_TIME()              0x00002633C0
#define DENM_EVENT_GET_POSITION_MAJOR_CONFIDENCE()  4094
#define DENM_EVENT_GET_POSITION_MINOR_CONFIDENCE()  4094
#define DENM_EVENT_GET_POSITION_MAJOR_ORIENTATION() 0
#define DENM_EVENT_GET_POSITION_ALT_VALUE()         (95.50)
#define DENM_EVENT_GET_POSITION_ALT_CONFIDENCE()    AltitudeConfidence_alt_050_00

/* clang-format on */


/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */
/* clang-format off */
#define DENM_SSP_CHECK(x,y) (((x) & (y)) ? 1 : 0)
#define DENM_SSP_LEN 4
/* SSP Definitions for Permissions in DENM */
/* Octet Position: 0 , SSP Version control */
/* Octet Position: 1 */
#define TRAFFIC_CONDITION                                   (1 << 7)
#define ACCIDENT                                            (1 << 6)
#define ROADWORKS                                           (1 << 5)
#define ADVERSE_WEATHER_CONDITION_ADHESION                  (1 << 4)
#define HAZARDOUS_LOCATION_SURFACE_CONDITION                (1 << 3)
#define HAZARDOUS_LOCATION_OBSTACLE_ON_THE_ROAD             (1 << 2)
#define HAZARDOUS_LOCATION_ANIMAL_ON_THE_ROAD               (1 << 1)
#define HUMAN_PRESENCE_ON_THE_ROAD                          (1 << 0)
/* Octet Position: 2 */
#define WRONG_WAY_DRIVING                                   (1 << 7)
#define RESCUE_AND_RECOVERY_WORK_IN_PROGRESS                (1 << 6)
#define ADVERSE_WEATHER_CONDITION_EXTREME_WEATHER_CONDITION (1 << 5)
#define ADVERSE_WEATHER_CONDITION_VISIBILITY                (1 << 4)
#define ADVERSE_WEATHER_CONDITION_PRECIPITATION             (1 << 3)
#define SLOW_VEHICLE                                        (1 << 2)
#define DANGEROUS_END_OF_QUEUE                              (1 << 1)
#define VEHICLE_BREAK_DOWN                                  (1 << 0)
/* Octet Position: 3 */
#define POST_CRASH                                          (1 << 7)
#define HUMAN_PROBLEM                                       (1 << 6)
#define STATIONARY_VEHICLE                                  (1 << 5)
#define EMERGENCY_VEHICLE_APPROACHING                       (1 << 4)
#define HAZARDOUS_LOCATION_DANGEROUS_CURVE                  (1 << 3)
#define COLLISION_RISK                                      (1 << 2)
#define SIGNAL_VIOLATION                                    (1 << 1)
#define DANGEROUS_SITUATION                                 (1 << 0)
/* clang-format on */
/*
 *******************************************************************************
 * Data type definition
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

static btp_handler_ptr btp_handler;
static DENM denm_tx_encode_fmt;


/*
 *******************************************************************************
 * Public functions declaration
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Private functions declaration
 *******************************************************************************
 */

static int32_t denm_encode(uint8_t **pp_denm_data, fix_data_t *p_fix_data);
static int32_t denm_decode(uint8_t *p_rx_payload, int32_t rx_payload_len, btp_handler_recv_indicator_t *p_recv_ind, bool ssp_check);

static void *denm_receiver_loop(void *p_param __attribute__((unused)));

static int32_t denm_check_msg_permission(DENM *p_denm_msg, uint8_t *p_ssp, uint32_t ssp_len);
/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
int32_t main()
{
    uint16_t src_port = DENM_BTP_PORT; /* Listen DENM message from remote ITS station. */
    uint16_t dest_port = DENM_BTP_PORT; /* Broadcast DENM message to remote ITS station. */
    uint8_t *p_tx_payload = NULL;
    int32_t tx_payload_len = 0;
    uint32_t send_count;
    int32_t result;

    poti_service_t *p_poti_service = NULL;
    fix_data_t poti_fix_data = FIX_DATA_INIT;

    btp_handler_ptr btp_handler;
    btp_handler_config_t btp_config;
    btp_handler_send_config_t send_config;
    pthread_t thread;

    printf("Hello, This is Unex's DENM (GBC) TX/RX example!\n");
    printf("The source port setting is %u!\n", src_port);
    printf("The destination port setting is %u!\n", dest_port);

    /* Register POTI service. */
    result = poti_create_service(&p_poti_service, NULL);

    if (IS_SUCCESS(result)) {
        printf("Create POTI service successfully\n");
    }
    else {
        printf("Cannot create POTI service: %s\n", ERROR_MSG(result));

        return -1;
    }

    /* BTP handler default configuration. */
    BTP_HDLR_CONFIG_INIT(&btp_config, dest_port);

#if (__EN_SECURITY_FEATURE__)
    /*
     * Enable AID security mode.
     * Please make sure all the security configuration in gnd.json is set correctly.
     */
    btp_config.security.type = GN_SEC_ENCAP_PKT_BY_AID_PROFILE;
    btp_config.security.its_aid = ITS_SEC_DEN_AID; /* Using DENM AID for example. */
#endif

    /* Create BTP handler. */
    result = btp_create(&btp_handler, &btp_config);

    if (IS_SUCCESS(result)) {
        /* Bind BTP port. */
        result = btp_bind(btp_handler, src_port);

        if (IS_SUCCESS(result)) {
            /* Activate BTP receiver thread. */
            pthread_create(&thread, NULL, denm_receiver_loop, NULL);

#if (__EN_SECURITY_FEATURE__)
            /*
            * Assign DENM service specific permissions according to the actual content in payload.
            * Please refer to ETSI TS 103 097, ETSI EN 302 637-3 for more details.
            * Please refer to Unex-APG-ETSI-GN-BTP for more information of build-in certificates
            */
            /* SSP Version control */
            send_config.security.ssp[0] = 0;
            /* Service-specific parameter : CauseCodeType */
            send_config.security.ssp[1] = 0;
            send_config.security.ssp[2] = WRONG_WAY_DRIVING;
            send_config.security.ssp[3] = 0;
            send_config.security.ssp_len = 4;
#endif

            /* Broadcast 10 DENM messages. */
            for (send_count = 0; send_count < 10; send_count++) {
                /* Wait for new GNSS update (10 Hz by default). */
                printf("Waiting for GNSS update\n");
                result = poti_wait_gnss_data(p_poti_service, NULL);

                if (IS_SUCCESS(result)) {
                    /* Get fix data. */
                    result = poti_get_fix_data(p_poti_service, &poti_fix_data);
                    if (IS_SUCCESS(result)) {
                        /* Make sure the navigation information is valid. */
                        if (poti_fix_data.status == FIX_STATUS_NA || poti_fix_data.mode == FIX_MODE_NA || poti_fix_data.mode == FIX_MODE_NO_FIX) {
                            printf("Navigation information is invalid\n");
                        }
                        else {
                            /* BTP send packet default configuration (GBC/BTP-B). */
                            BTP_SEND_CONFIG_INIT(&send_config);

                            /* Must transmit DENM by using GBC mechanism, so set TX network header type to GBC. */
                            GN_ENABLE_OPTIONAL_CONFIG(send_config.gn_optional.bitmask, GN_OPTION_NETWORK_PKT_TYPE_BIT);
                            send_config.gn_optional.type = GN_SVC_NETWORK_PKT_GBC;

                            /* Setup destination area according to current position. */
                            GN_ENABLE_OPTIONAL_CONFIG(send_config.gn_optional.bitmask, GN_OPTION_DEST_AREA_BIT);
                            send_config.gn_optional.dest_area.area_type = GN_AREA_TYPE_RECT; /* GN_AREA_TYPE_CIRCLE, GN_AREA_TYPE_RECT, GN_AREA_TYPE_ELIP. */
                            send_config.gn_optional.dest_area.pos_latitude = (int32_t)(poti_fix_data.latitude * 10000000.0); /* WGS84, 1/10 micros degrees. */
                            send_config.gn_optional.dest_area.pos_longitude = (int32_t)(poti_fix_data.longitude * 10000000.0); /* WGS84, 1/10 micros degrees. */
                            send_config.gn_optional.dest_area.distance_a = 100; /* Distance a of the geometric shape, meters. */
                            send_config.gn_optional.dest_area.distance_b = 100; /* Distance a of the geometric shape, meters. */
                            send_config.gn_optional.dest_area.angle = 0; /* Angle of the geometric shape, degrees from North. */

                            /* Generate DENM message. */
                            tx_payload_len = denm_encode(&p_tx_payload, &poti_fix_data);

                            if (p_tx_payload != NULL && tx_payload_len > 0) {
                                /* Broadcast the DENM message (BTP-B, GBC). */
                                result = btp_send(btp_handler, &send_config, p_tx_payload, tx_payload_len);

                                if (result > 0) {
                                    printf("Transmitted %d bytes data\n", result);
                                }
                                else {
                                    printf("Transmitting error: %s\n", ERROR_MSG(result));
                                }

                                /* Release the allocated message buffer. */
                                itsmsg_buf_free(p_tx_payload);
                                p_tx_payload = NULL;
                                tx_payload_len = 0;
                            }
                            else {
                                printf("Cannot generate DENM message\n");
                            }
                        }
                    }
                    else {
                        printf("Cannot get GNSS fix data: %s\n", ERROR_MSG(result));
                    }
                }
                else {
                    printf("poti_wait_gnss_data error: %s\n", ERROR_MSG(result));
                }

                fflush(stdout);
            }

            /* Wait till denm_receiver_loop is completed. */
            printf("Wait till denm_receiver_loop is completed\n");
            pthread_join(thread, NULL);
        }
        else {
            printf("Bind error: %s\n", ERROR_MSG(result));
        }

        /* Release BTP handler. */
        btp_release(btp_handler);

        /* Release POTI handler. */
        poti_release_service(p_poti_service);
    }
    else {
        printf("Cannot create BTP handler: %s\n", ERROR_MSG(result));
    }

    return 0;
}


/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static int32_t denm_encode(uint8_t **pp_denm_data, fix_data_t *p_fix_data)
{
    ITSMsgCodecErr err;
    int buf_len = 0;

    /* Make sure we reset the data structure at least once. */
    memset((void *)&denm_tx_encode_fmt, 0, sizeof(denm_tx_encode_fmt));

    /* For the present document, the value of the DE protocolVersion shall be set to 1.  */
    denm_tx_encode_fmt.header.protocolVersion = DENM_PROTOCOL_VERSION;
    denm_tx_encode_fmt.header.messageID = DENM_Id;
    denm_tx_encode_fmt.header.stationID = DENM_STATION_ID_DEF;

    /* Identifier generated by the DEN basic service for new DENM. */
    denm_tx_encode_fmt.denm.management.actionID.originatingStationID = DENM_STATION_ID_DEF; /* StationID (0..4294967295) */
    denm_tx_encode_fmt.denm.management.actionID.sequenceNumber = DENM_ACTION_SEQUENCE(); /* SequenceNumber (0..65535) */

    /*
     * Time at which the event is detected by the originating ITS-S. For the DENM repetition, this DE shall remain unchanged.
     * For the DENM update, this DE shall be the time at which the event update is detected.
     * For the DENM termination, this DE shall be the time at which the termination of the event is detected.
     *
     * Number of milliseconds since 2004-01-01T00:00:00.000Z, as specified in ISO 8601 [i.10].
     */
    asn1_new_integer_u64(&denm_tx_encode_fmt.denm.management.detectionTime, (uint64_t)DENM_EVENT_GET_POSITION_TIME());

    /*
     * This DE refers to the time at which a new DENM, an update DENM or a cancellation DENM is generated.
     *
     * Number of milliseconds since 2004-01-01T00:00:00.000Z, as specified in ISO 8601 [i.10].
     */
    asn1_new_integer_u64(&denm_tx_encode_fmt.denm.management.referenceTime, (uint64_t)fmod(p_fix_data->time.tai_since_2004 * 1000.0, 65536));

    /* This DE indicates if the type of generated DENM is a cancellation DENM or a negation DENM. */
    denm_tx_encode_fmt.denm.management.termination_option = FALSE; /* Send DENM_trigger for demonstration, no need to enable termination field. */
    //denm_tx_encode_fmt.denm.management.termination =

    /* Geographical position of the detected event. */
    denm_tx_encode_fmt.denm.management.eventPosition.latitude = (int32_t)(DENM_EVENT_GET_POSITION_LAT() * 10000000.0); /* Convert to 1/10 micro degree. */
    denm_tx_encode_fmt.denm.management.eventPosition.longitude = (int32_t)(DENM_EVENT_GET_POSITION_LONG() * 10000000.0); /* Convert to 1/10 micro degree. */
    denm_tx_encode_fmt.denm.management.eventPosition.positionConfidenceEllipse.semiMajorConfidence = DENM_EVENT_GET_POSITION_MAJOR_CONFIDENCE(); /* Convert to centimetre. */
    denm_tx_encode_fmt.denm.management.eventPosition.positionConfidenceEllipse.semiMinorConfidence = DENM_EVENT_GET_POSITION_MINOR_CONFIDENCE(); /* Convert to centimetre. */
    denm_tx_encode_fmt.denm.management.eventPosition.positionConfidenceEllipse.semiMajorOrientation = (int32_t)(DENM_EVENT_GET_POSITION_MAJOR_ORIENTATION() * 10.0); /* Convert to 0.1 degree from North. */
    denm_tx_encode_fmt.denm.management.eventPosition.altitude.altitudeValue = (int32_t)(p_fix_data->altitude * 100.0); /* Convert to 0.01 metre. */
    denm_tx_encode_fmt.denm.management.eventPosition.altitude.altitudeConfidence = DENM_EVENT_GET_POSITION_ALT_CONFIDENCE();

    /* The distance within which the event is considered relevant to the receiving ITS-S. */
    denm_tx_encode_fmt.denm.management.relevanceDistance_option = FALSE;
    //denm_tx_encode_fmt.denm.management.relevanceDistance =

    /* The traffic direction along which the event information is relevant for the receiving ITS-S. */
    denm_tx_encode_fmt.denm.management.relevanceTrafficDirection_option = FALSE;
    //denm_tx_encode_fmt.denm.management.relevanceTrafficDirection =

    /* Validity duration of a DENM. */
    denm_tx_encode_fmt.denm.management.validityDuration = DENM_VALIDITY_DURATION_DEF; /* ValidityDuration (0..86400) */

    /* Time interval for DENM transmission as defined by the originating ITS-S. */
    denm_tx_encode_fmt.denm.management.transmissionInterval_option = FALSE;
    //denm_tx_encode_fmt.denm.management.transmissionInterval = /* TransmissionInterval (1..10000) */

    /* This DE provides the station type information of the originating ITS-S. */
    denm_tx_encode_fmt.denm.management.stationType = DENM_STATION_TYPE_DEF;

    /*
     *  The situation container includes information that describes the detected event.
     */
    denm_tx_encode_fmt.denm.situation_option = TRUE;
    denm_tx_encode_fmt.denm.situation.informationQuality = 0; /* InformationQuality (0..7), If the information is unknown, the DE shall be set to 0. */
    denm_tx_encode_fmt.denm.situation.eventType.causeCode = CauseCodeType_wrongWayDriving;
    denm_tx_encode_fmt.denm.situation.eventType.subCauseCode = WrongWayDrivingSubCauseCode_wrongLane;

    denm_tx_encode_fmt.denm.situation.linkedCause_option = FALSE;
    //denm_tx_encode_fmt.denm.situation.linkedCause =
    denm_tx_encode_fmt.denm.situation.eventHistory_option = FALSE;
    //denm_tx_encode_fmt.denm.situation.eventHistory =

    /*
     * The location container describes the location of the detected event.
     */
    denm_tx_encode_fmt.denm.location_option = TRUE;
    denm_tx_encode_fmt.denm.location.eventSpeed_option = TRUE;
    denm_tx_encode_fmt.denm.location.eventSpeed.speedValue = SpeedValue_unavailable; /* 0,01 m/s */
    denm_tx_encode_fmt.denm.location.eventSpeed.speedConfidence = SpeedConfidence_equalOrWithinOneMeterPerSec;
    denm_tx_encode_fmt.denm.location.eventPositionHeading_option = FALSE;
    //denm_tx_encode_fmt.denm.location.eventPositionHeading =

    /* Allocate and initialize each entries and paths. */
    denm_tx_encode_fmt.denm.location.traces.count = 1;
    denm_tx_encode_fmt.denm.location.traces.tab = (PathHistory_ITS *)calloc(sizeof(PathHistory_ITS), denm_tx_encode_fmt.denm.location.traces.count);
    denm_tx_encode_fmt.denm.location.traces.tab[0].count = 2;
    denm_tx_encode_fmt.denm.location.traces.tab[0].tab = (PathPoint *)calloc(sizeof(PathPoint), denm_tx_encode_fmt.denm.location.traces.tab[0].count);
    denm_tx_encode_fmt.denm.location.traces.tab[0].tab[0].pathPosition.deltaLatitude = -123010;
    denm_tx_encode_fmt.denm.location.traces.tab[0].tab[0].pathPosition.deltaLongitude = -131068;
    denm_tx_encode_fmt.denm.location.traces.tab[0].tab[0].pathPosition.deltaAltitude = -12693;
    denm_tx_encode_fmt.denm.location.traces.tab[0].tab[1].pathPosition.deltaLatitude = -131058;
    denm_tx_encode_fmt.denm.location.traces.tab[0].tab[1].pathPosition.deltaLongitude = -131055;
    denm_tx_encode_fmt.denm.location.traces.tab[0].tab[1].pathPosition.deltaAltitude = -11769;
    denm_tx_encode_fmt.denm.location.traces.tab[0].tab[1].pathDeltaTime_option = TRUE;
    denm_tx_encode_fmt.denm.location.traces.tab[0].tab[1].pathDeltaTime = -151310;

    denm_tx_encode_fmt.denm.location.roadType_option = FALSE;
    //denm_tx_encode_fmt.denm.location.roadType

    /*
     * The a la carte container contains additional information that is not provided by other containers.
     */
    denm_tx_encode_fmt.denm.alacarte_option = FALSE;
    //denm_tx_encode_fmt.denm.alacarte.lanePosition_option =
    //denm_tx_encode_fmt.denm.alacarte.lanePosition =
    //denm_tx_encode_fmt.denm.alacarte.impactReduction_option =
    //denm_tx_encode_fmt.denm.alacarte.externalTemperature_option =
    //denm_tx_encode_fmt.denm.alacarte.externalTemperature =
    //denm_tx_encode_fmt.denm.alacarte.roadWorks_option =
    //denm_tx_encode_fmt.denm.alacarte.roadWorks =
    //denm_tx_encode_fmt.denm.alacarte.positioningSolution_option =
    //denm_tx_encode_fmt.denm.alacarte.positioningSolution =
    //denm_tx_encode_fmt.denm.alacarte.stationaryVehicle_option =
    //denm_tx_encode_fmt.denm.alacarte.stationaryVehicle =

    /* Allocate a buffer for restoring the decode error information if needed. */
    err.msg_size = 256;
    err.msg = calloc(1, err.msg_size);

    if (err.msg != NULL) {
        /* Encode the DENM message. */
        buf_len = itsmsg_encode(pp_denm_data, (ItsPduHeader *)&denm_tx_encode_fmt, &err);

        if (buf_len <= 0) {
            printf("itsmsg_encode error: %s\n", err.msg);
        }

        /* Release the allocated error message buffer. */
        free(err.msg);
    }
    else {
        printf("Cannot allocate memory for error message buffer.\n");
    }

    /* Release allocated memory. */
    free(denm_tx_encode_fmt.denm.location.traces.tab[0].tab);
    free(denm_tx_encode_fmt.denm.location.traces.tab);
    asn1_free_integer(&denm_tx_encode_fmt.denm.management.detectionTime);
    asn1_free_integer(&denm_tx_encode_fmt.denm.management.referenceTime);

    return buf_len;
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static int32_t denm_decode(uint8_t *p_rx_payload, int32_t rx_payload_len, btp_handler_recv_indicator_t *p_recv_ind, bool ssp_check)
{
    ITSMsgCodecErr err;
    ItsPduHeader *p_rx_decode_hdr = NULL;
    DENM *p_rx_decode_denm = NULL;
    uint64_t detection_time;
    uint64_t reference_time;
    int32_t result;

    if (p_recv_ind == NULL || p_rx_payload == NULL || rx_payload_len == 0) {
        return -1;
    }

    /* Allocate a buffer for restoring the decode error information if needed. */
    err.msg_size = 256;
    err.msg = calloc(1, err.msg_size);

    if (err.msg == NULL) {
        printf("Cannot allocate memory for error message buffer.\n");
        return -1;
    }

    /* Determine and decode the content in RX payload. */
    result = itsmsg_decode(&p_rx_decode_hdr, p_rx_payload, rx_payload_len, &err);

    /* Check whether this is a ITS message. */
    if (result > 0 && p_rx_decode_hdr != NULL) {
        /* Check whether this is a ITS DENM message. */
        if (p_rx_decode_hdr->messageID == DENM_Id) {
            /* Convert to DENM data format. */
            p_rx_decode_denm = (DENM *)p_rx_decode_hdr;

            if (ssp_check) {
                /* Check DENM msg permission */
                result = denm_check_msg_permission(p_rx_decode_denm, p_recv_ind->security.ssp, p_recv_ind->security.ssp_len);
                printf("\tCheck msg permissions: ");
                if (IS_SUCCESS(result)) {
                    printf("trustworthy\n");
                }
                else {
                    printf("untrustworthy\n");
                }
            }

            /* Display the decoded DENM content. */
            printf("[ Received DENM from station %d ]\n", p_rx_decode_denm->header.stationID);
            printf("\tSource LL_ADDR: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   p_recv_ind->src_pv.addr.mid[0], p_recv_ind->src_pv.addr.mid[1],
                   p_recv_ind->src_pv.addr.mid[2], p_recv_ind->src_pv.addr.mid[3],
                   p_recv_ind->src_pv.addr.mid[4], p_recv_ind->src_pv.addr.mid[5]);
            printf("\toriginatingStationID: %u\n", p_rx_decode_denm->denm.management.actionID.originatingStationID);
            printf("\tsequenceNumber: %u\n", p_rx_decode_denm->denm.management.actionID.sequenceNumber);

            asn1_get_integer_si64_ov(&p_rx_decode_denm->denm.management.detectionTime, (int64_t *)&detection_time);
            printf("\tdetectionTime: %" PRIu64 "\n", detection_time);

            asn1_get_integer_si64_ov(&p_rx_decode_denm->denm.management.referenceTime, (int64_t *)&reference_time);
            printf("\treferenceTime: %" PRIu64 "\n", reference_time);

            printf("\tEvent latitude: %d\n", p_rx_decode_denm->denm.management.eventPosition.latitude);
            printf("\tEvent longitude: %d\n", p_rx_decode_denm->denm.management.eventPosition.longitude);

            if (p_rx_decode_denm->denm.situation_option) {
                printf("\tcauseCode: %d\n", p_rx_decode_denm->denm.situation.eventType.causeCode);
                printf("\tcauseCode: %d\n", p_rx_decode_denm->denm.situation.eventType.subCauseCode);
            }
        }
        else {
            printf("Received unrecognized ITS message type: %d\n", p_rx_decode_hdr->messageID);
        }

        /* Release the decode message buffer. */
        itsmsg_free(p_rx_decode_hdr);
    }
    else {
        printf("Unable to decode RX packet: %s\n", err.msg);
    }

    /* Release the allocated error message buffer. */
    free(err.msg);

    return 0;
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static void *denm_receiver_loop(void *p_param __attribute__((unused)))
{
    uint8_t rx_payload[GN_MAX_SDU_SIZE];
    int32_t result;
    int32_t rx_count;
    btp_handler_recv_indicator_t recv_ind;
    bool ssp_check = false;

    printf("DENM receiver start\n");

    for (rx_count = 0; rx_count < 10; rx_count++) {
        /* Listen the DENM BTP port. */
        result = btp_recv(btp_handler, &recv_ind, rx_payload, sizeof(rx_payload), BTP_RECV_WAIT_FOREVER);

        if (result > 0) {
            printf("Received %d bytes data\n", result);

            if (IS_DECAP_FAIL(recv_ind.security.status) == true) {
                printf("\tSecurity status: decapsulation error (%d), the payload content is untrustworthy\n", recv_ind.security.status);
            }
            else {
                if (recv_ind.security.status == ITS_SEC_SUCCESS) {
                    printf("\tSecurity status: this packet is decapsulated with no error\n");
                    printf("\tITS-AID: %u\n", recv_ind.security.its_aid);
                    ssp_check = true;
                }
                else if (recv_ind.security.status == ITS_SEC_NA) {
                    printf("\tSecurity status: no security protection\n");
                }
                else {
                    printf("\tSecurity status: other (%d)\n", recv_ind.security.status);
                }
                /* Try to decode the received message. */
                denm_decode(rx_payload, result, &recv_ind, ssp_check);
            }
        }
        else {
            printf("Receiving error: %s\n", ERROR_MSG(result));

            /* Do error handling if needed. */
            sleep(1);
        }

        fflush(stdout);
    }

    /* Terminate this thread. */
    pthread_exit(NULL);
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static int32_t denm_check_msg_permission(DENM *p_denm_msg, uint8_t *p_ssp, uint32_t ssp_len)
{
    int32_t rc = 0;
    if (ssp_len < DENM_SSP_LEN) {
        rc = -1;
        printf("Err: SSP length[%d] is not enough\n", ssp_len);
        goto FAILURE;
    }

    if (p_denm_msg->denm.situation_option) {
        /*
         *   For example, only check wrongWayDriving
         *   Please refer to ETSI EN 302 637-3 to check related SSP item
         */
        switch (p_denm_msg->denm.situation.eventType.causeCode) {
            case CauseCodeType_wrongWayDriving:
                if (DENM_SSP_CHECK(WRONG_WAY_DRIVING, p_ssp[2]) == false) {
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
