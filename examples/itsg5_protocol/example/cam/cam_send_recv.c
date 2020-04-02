/**
 *******************************************************************************
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * This example program is created for demonstrating how to perform CAM (Cooperative
 * awareness basic) service packet sending and receiving procedure which are defined in
 * ETSI EN 302 637-2 V1.3.1 (2014-09), Intelligent Transport Systems (ITS); Vehicular
 * Communications; Basic Set of Applications; Part 2: Specification of Cooperative
 * Awareness Basic Service standard document.
 *
 * Usage:
 *      (1) Correct the toolchain setting then type 'make' to build this example program.
 *      (2) Upload the 'cam_send_recv' execution file to 2 different Unex V2X modules.
 *      (3) Run the 'cam_send_recv' on module A and module B, then check whether these 2
 *          devices received CAM message from each other.
 *
 * @file    cam_send_recv.c
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

#define CAM_BTP_PORT                                2001    /* ETSI TS 103 248. */

#define CAM_PROTOCOL_VERSION                        1       /* ETSI EN 302 637-2 V1.3.1 (2014-09) */

/* Please assign ID for your ITS station. */
#define CAM_STATION_ID_DEF                          168

/* Please assign the type of your ITS station. */
#define CAM_STATION_TYPE_DEF                        GN_ITS_STATION_SPECIAL_VEHICLE

/* Please correct following macros for getting data from additional INS sensors on host system. */
#define CAM_SENSOR_GET_DRIVE_DIRECTION()            DriveDirection_forward

#define CAM_SENSOR_GET_VEHICLE_LENGTH_VALUE()       38      /* 0.1 metre. */
#define CAM_SENSOR_GET_VEHICLE_LENGTH_CONF()        VehicleLengthConfidenceIndication_noTrailerPresent

#define CAM_SENSOR_GET_VEGICLE_WIDTH_VALUE()        18      /* 0.1 metre. */

#define CAM_SENSOR_GET_LONG_ACCEL_VALUE()           0       /* 0.1 m/s^2. */
#define CAM_SENSOR_GET_LONG_ACCEL_CONF()            102     /* 1 ~ 102 */

#define CAM_SENSOR_GET_CURVATURE_VALUE()            0       /* Curvature, 1 over 30000 meters, (-30000 .. 30001) */
#define CAM_SENSOR_GET_CURVATURE_CONF()             7       /* 0 ~ 7. */

#define CAM_SENSOR_GET_CURVATURE_CONF_CAL_MODE()    CurvatureCalculationMode_unavailable

#define CAM_SENSOR_GET_YAW_RATE_VALUE()             0       /* 0,01 degree per second. */
#define CAM_SENSOR_GET_YAW_RATE_CONF()              YawRateConfidence_unavailable_ITS

/* clang-format on */

/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */
/* clang-format off */
#define CAM_SSP_CHECK(x,y) (((x) & (y)) ? 1 : 0)
#define CAM_SSP_LEN 3
/* SSP Definitions for Permissions in CAM */
/* Octet Position: 0 , SSP Version control */
/* Octet Position: 1 */
#define CEN_DSRC_TOLLING_ZONE                           (1 << 7)
#define PUBLIC_TRANSPORT                                (1 << 6)
#define SPECIAL_TRANSPORT                               (1 << 5)
#define DANGEROUS_GOODS                                 (1 << 4)
#define ROADWORK                                        (1 << 3)
#define RESCUE                                          (1 << 2)
#define EMERGENCY                                       (1 << 1)
#define SAFETY_CAR                                      (1 << 0)
/* Octet Position: 2 */
#define CLOSED_LANES                                    (1 << 7)
#define REQUEST_FOR_RIGHT_OF_WAY                        (1 << 6)
#define REQUEST_FOR_FREE_CROSSING_AT_A_TRAFFIC_LIGHT    (1 << 5)
#define NO_PASSING                                      (1 << 4)
#define NO_PASSING_FOR_TRUCKS                           (1 << 3)
#define SPEEED_LIMIT                                    (1 << 2)
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
static CAM cam_tx_encode_fmt;


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

static int32_t cam_encode(uint8_t **pp_cam_data, fix_data_t *p_fix_data);
static int32_t cam_decode(uint8_t *p_rx_payload, int32_t rx_payload_len, btp_handler_recv_indicator_t *p_recv_ind, bool ssp_check);

static void *cam_receiver_loop(void *p_param __attribute__((unused)));

static int32_t cam_set_semi_axis_length(double meter);
static int32_t cam_set_altitude_confidence(double metre);
static int32_t cam_set_heading_confidence(double degree);
static int32_t cam_set_speed_confidence(double meter_per_sec);

static int32_t cam_check_msg_permission(CAM *p_cam_msg, uint8_t *p_ssp, uint32_t ssp_len);
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
int32_t main(int argc, char **argv)
{
    if(0 == argc) {

        return -1;
    }

    uint16_t src_port = CAM_BTP_PORT; /* Listen CAM message from remote ITS station. */
    uint16_t dest_port = CAM_BTP_PORT; /* Broadcast CAM message to remote ITS station. */
    uint8_t *p_tx_payload = NULL;
    int32_t tx_payload_len = 0;
    uint32_t send_count = 0;
    uint32_t send_minor_count = 0;
    int32_t result;

    poti_service_t *p_poti_service = NULL;
    fix_data_t poti_fix_data = FIX_DATA_INIT;

    btp_handler_ptr btp_handler;
    btp_handler_config_t btp_config;
    btp_handler_send_config_t send_config;
    pthread_t thread;

    // Tushy
    // Internal list of lla data.
    uint32_t lla_array_ix = 0;
    uint32_t lla_array_size = 8;
    double latitude_array[lla_array_size];
    double longitude_array[lla_array_size];
    if(1 == argv[0]) {

        latitude_array[0] = 52.4965207;
        latitude_array[1] = 52.4965664;
        latitude_array[2] = 52.4965958;
        latitude_array[3] = 52.4966317;
        latitude_array[4] = 52.4966579;
        latitude_array[5] = 52.4966873;
        latitude_array[6] = 52.4967166;
        latitude_array[7] = 52.4967362;

        longitude_array[0] = 13.3861558;
        longitude_array[1] = 13.3858661;
        longitude_array[2] = 13.3856247;
        longitude_array[3] = 13.3853833;
        longitude_array[4] = 13.3851634;
        longitude_array[5] = 13.3849756;
        longitude_array[6] = 13.3848308;
        longitude_array[7] = 13.3846055;

    } else {

        latitude_array[0] = 52.4976768;
        latitude_array[1] = 52.4975364;
        latitude_array[2] = 52.4974384;
        latitude_array[3] = 52.4973306;
        latitude_array[4] = 52.4972163;
        latitude_array[5] = 52.4970661;
        latitude_array[6] = 52.4969191;
        latitude_array[7] = 52.4967754;

        longitude_array[0] = 13.3849059;
        longitude_array[1] = 13.384863;
        longitude_array[2] = 13.3848254;
        longitude_array[3] = 13.3847825;
        longitude_array[4] = 13.3847503;
        longitude_array[5] = 13.3846967;
        longitude_array[6] = 13.3846538;
        longitude_array[7] = 13.3846055;
    }

    printf("Hello, This is Unex's CAM (SHB) TX/RX example!\n");
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
    btp_config.security.its_aid = ITS_SEC_CAM_AID; /* Using CAM AID for example. */
#endif
    /* Create BTP handler. */
    result = btp_create(&btp_handler, &btp_config);

    if (IS_SUCCESS(result)) {
        /* Bind BTP port. */
        result = btp_bind(btp_handler, src_port);

        if (IS_SUCCESS(result)) {
            /* Activate BTP receiver thread. */
            pthread_create(&thread, NULL, cam_receiver_loop, NULL);

            /* BTP send packet default configuration (SHB/BTP-B). */
            BTP_SEND_CONFIG_INIT(&send_config);

#if (__EN_SECURITY_FEATURE__)
            /*
            * Assign CAM service specific permissions according to the actual content in payload.
            * Please refer to ETSI TS 103 097, ETSI EN 302 637-2 for more details.
            * Please refer to Unex-APG-ETSI-GN-BTP for more information of build-in certificates
            */
            /* SSP Version control */
            send_config.security.ssp[0] = 0x0;
            /* Service-specific parameter */
            send_config.security.ssp[1] = EMERGENCY; /* Emergency container */
            send_config.security.ssp[2] = REQUEST_FOR_FREE_CROSSING_AT_A_TRAFFIC_LIGHT; /* EmergencyPriority */
            send_config.security.ssp_len = 3;
#endif

            /* Broadcast 10 CAM messages. */
            //for (send_count = 0; send_count < 10; send_count++) { //Tushy
            while(send_count == 0) {
                /* Wait for new GNSS update (10 Hz by default). */
                printf("Waiting for GNSS update\n");
                result = poti_wait_gnss_data(p_poti_service, NULL);

                send_minor_count++;

                if(10 <= send_minor_count) {

                    send_minor_count = 0;

                    if (IS_SUCCESS(result)) {

                        /* Get fix data. */
                        result = poti_get_fix_data(p_poti_service, &poti_fix_data);
                        if (IS_SUCCESS(result)) {
                            /* Make sure the navigation information is valid. */
                            if (poti_fix_data.status == FIX_STATUS_NA || poti_fix_data.mode == FIX_MODE_NA || poti_fix_data.mode == FIX_MODE_NO_FIX) {
                                printf("Navigation information is invalid\n");
                            }
                            else {

                                poti_fix_data.longitude = longitude_array[lla_array_ix];
                                poti_fix_data.latitude = latitude_array[lla_array_ix];
                                poti_fix_data.altitude = 0;

                                lla_array_ix++;

                                lla_array_ix %= lla_array_size;

                                /* Generate CAM message. */
                                tx_payload_len = cam_encode(&p_tx_payload, &poti_fix_data);

                                if (p_tx_payload != NULL && tx_payload_len > 0) {
                                    /* Broadcast the CAM message (BTP-B, SHB). */
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
                                    printf("Cannot generate CAM message\n");
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
                }

                fflush(stdout);
            }

            /* Wait till cam_receiver_loop is completed. */
            printf("Wait till cam_receiver_loop is completed\n");
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
static int32_t cam_encode(uint8_t **pp_cam_data, fix_data_t *p_fix_data)
{
    ITSMsgCodecErr err;
    int buf_len = 0;

    /* Make sure we reset the data structure at least once. */
    memset((void *)&cam_tx_encode_fmt, 0, sizeof(cam_tx_encode_fmt));

    /* For the present document, the value of the DE protocolVersion shall be set to 1.  */
    cam_tx_encode_fmt.header.protocolVersion = CAM_PROTOCOL_VERSION;
    cam_tx_encode_fmt.header.messageID = CAM_Id;
    cam_tx_encode_fmt.header.stationID = CAM_STATION_ID_DEF;

    /*
     * Time corresponding to the time of the reference position in the CAM, considered
     * as time of the CAM generation.
     * The value of the DE shall be wrapped to 65 536. This value shall be set as the
     * remainder of the corresponding value of TimestampIts divided by 65 536 as below:
     *      generationDeltaTime = TimestampIts mod 65 536
     * TimestampIts represents an integer value in milliseconds since 2004-01-01T00:00:00:000Z
     * as defined in ETSI TS 102 894-2 [2].
     */
    cam_tx_encode_fmt.cam.generationDeltaTime = (int32_t)fmod(p_fix_data->time.tai_since_2004 * 1000.0, 65536); /* TAI milliseconds since 2004-01-01 00:00:00.000 UTC. */

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
    cam_tx_encode_fmt.cam.camParameters.basicContainer.stationType = CAM_STATION_TYPE_DEF;
    cam_tx_encode_fmt.cam.camParameters.basicContainer.referencePosition.latitude = (int32_t)(p_fix_data->latitude * 10000000.0); /* Convert to 1/10 micro degree. */
    cam_tx_encode_fmt.cam.camParameters.basicContainer.referencePosition.longitude = (int32_t)(p_fix_data->longitude * 10000000.0); /* Convert to 1/10 micro degree. */
    cam_tx_encode_fmt.cam.camParameters.basicContainer.referencePosition.positionConfidenceEllipse.semiMajorConfidence = cam_set_semi_axis_length(p_fix_data->err_smajor_axis); /* Convert to centimetre. */
    cam_tx_encode_fmt.cam.camParameters.basicContainer.referencePosition.positionConfidenceEllipse.semiMinorConfidence = cam_set_semi_axis_length(p_fix_data->err_sminor_axis); /* Convert to centimetre. */
    cam_tx_encode_fmt.cam.camParameters.basicContainer.referencePosition.positionConfidenceEllipse.semiMajorOrientation = (int32_t)(p_fix_data->err_smajor_orientation * 10.0); /* Convert to 0.1 degree from North. */
    cam_tx_encode_fmt.cam.camParameters.basicContainer.referencePosition.altitude.altitudeValue = (int32_t)(p_fix_data->altitude * 100.0); /* Convert to 0.01 metre. */
    cam_tx_encode_fmt.cam.camParameters.basicContainer.referencePosition.altitude.altitudeConfidence = cam_set_altitude_confidence(p_fix_data->err_altitude);

    /*
     * The mandatory high frequency container of CAM.
     */

    /* Heading. */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.heading.headingValue = (int32_t)(p_fix_data->course_over_ground * 10.0); /* Convert to 0.1 degree from North. */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.heading.headingConfidence = cam_set_heading_confidence(p_fix_data->err_course_over_ground); /* Convert to 1 ~ 127 enumeration. */

    /* Speed, 0.01 m/s */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedValue = (int16_t)(p_fix_data->horizontal_speed * 100.0); /* Convert to 0.01 metre per second. */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedConfidence = cam_set_speed_confidence(p_fix_data->err_horizontal_speed);

    /* Direction. */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.driveDirection = CAM_SENSOR_GET_DRIVE_DIRECTION();

    /* Vehicle length, 0.1 metre  */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue = CAM_SENSOR_GET_VEHICLE_LENGTH_VALUE();
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthConfidenceIndication = CAM_SENSOR_GET_VEHICLE_LENGTH_CONF();

    /* Vehicle width, 0.1 metre */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleWidth = CAM_SENSOR_GET_VEGICLE_WIDTH_VALUE();

    /* Longitudinal acceleration, 0.1 m/s^2 */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.longitudinalAcceleration.longitudinalAccelerationValue = CAM_SENSOR_GET_LONG_ACCEL_VALUE();
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.longitudinalAcceleration.longitudinalAccelerationConfidence = CAM_SENSOR_GET_LONG_ACCEL_CONF();

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
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.curvature.curvatureValue = CAM_SENSOR_GET_CURVATURE_VALUE();
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.curvature.curvatureConfidence = CAM_SENSOR_GET_CURVATURE_CONF();
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.curvatureCalculationMode = CAM_SENSOR_GET_CURVATURE_CONF_CAL_MODE();

    /* YAW rate, 0,01 degree per second. */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.yawRate.yawRateValue = CAM_SENSOR_GET_YAW_RATE_VALUE();
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.yawRate.yawRateConfidence = CAM_SENSOR_GET_YAW_RATE_CONF();

    /* Optional fields, disable all by default. */
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.accelerationControl_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.accelerationControl =
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.lanePosition_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.lanePosition =
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.steeringWheelAngle_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.steeringWheelAngle =
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.lateralAcceleration_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.lateralAcceleration =
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.verticalAcceleration_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.verticalAcceleration =
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.performanceClass_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.performanceClass =
    cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.cenDsrcTollingZone_option = FALSE;
    //cam_tx_encode_fmt.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.cenDsrcTollingZone =


    /*
    *   If stationType is set to specialVehicles(10)
    *       LowFrequencyContainer shall be set to BasicVehicleContainerLowFrequency
    *       SpecialVehicleContainer shall be set to EmergencyContainer
    */
    if (cam_tx_encode_fmt.cam.camParameters.basicContainer.stationType == GN_ITS_STATION_SPECIAL_VEHICLE) {
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
        cam_tx_encode_fmt.cam.camParameters.lowFrequencyContainer_option = true;
        cam_tx_encode_fmt.cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.vehicleRole = VehicleRole_emergency;
        /* alloc and set the bit of bitstring */
        asn1_bstr_alloc_set_bit(&(cam_tx_encode_fmt.cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.exteriorLights), ExteriorLights_MAX_BITS_ITS, ExteriorLights_highBeamHeadlightsOn_ITS);
        cam_tx_encode_fmt.cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.pathHistory.count = 0;

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
        cam_tx_encode_fmt.cam.camParameters.specialVehicleContainer_option = true;
        cam_tx_encode_fmt.cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_emergencyContainer;
        /* alloc and set the bit of bitstring */
        asn1_bstr_alloc_set_bit(&(cam_tx_encode_fmt.cam.camParameters.specialVehicleContainer.u.emergencyContainer.lightBarSirenInUse), LightBarSirenInUse_MAX_BITS, LightBarSirenInUse_sirenActivated);
        cam_tx_encode_fmt.cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority_option = true;
        asn1_bstr_alloc_set_bit(&(cam_tx_encode_fmt.cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority), EmergencyPriority_MAX_BITS, EmergencyPriority_requestForFreeCrossingAtATrafficLight);
        cam_tx_encode_fmt.cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication_option = false;
    }


    /* Allocate a buffer for restoring the decode error information if needed. */
    err.msg_size = 512;
    err.msg = calloc(1, err.msg_size);

    if (err.msg != NULL) {
        /* Encode the CAM message. */
        buf_len = itsmsg_encode(pp_cam_data, (ItsPduHeader *)&cam_tx_encode_fmt, &err);

        if (buf_len <= 0) {
            printf("itsmsg_encode error: %s\n", err.msg);
        }

        /* Release the allocated error message buffer. */
        free(err.msg);
    }
    else {
        printf("Cannot allocate memory for error message buffer.\n");
    }

    if (cam_tx_encode_fmt.cam.camParameters.basicContainer.stationType == GN_ITS_STATION_SPECIAL_VEHICLE) {
        /* free the memory for encoding */
        asn1_bstr_free(&(cam_tx_encode_fmt.cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.exteriorLights));
        asn1_bstr_free(&(cam_tx_encode_fmt.cam.camParameters.specialVehicleContainer.u.emergencyContainer.lightBarSirenInUse));
        asn1_bstr_free(&(cam_tx_encode_fmt.cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority));
    }
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
static int32_t cam_decode(uint8_t *p_rx_payload, int32_t rx_payload_len, btp_handler_recv_indicator_t *p_recv_ind, bool ssp_check)
{
    ITSMsgCodecErr err;
    ItsPduHeader *p_rx_decode_hdr = NULL;
    CAM *p_rx_decode_cam = NULL;
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
        /* Check whether this is a ITS CAM message. */
        if (p_rx_decode_hdr->messageID == CAM_Id) {
            /* Convert to CAM data format. */
            p_rx_decode_cam = (CAM *)p_rx_decode_hdr;

            if (ssp_check) {
                /* Check CAM msg permission */
                result = cam_check_msg_permission(p_rx_decode_cam, p_recv_ind->security.ssp, p_recv_ind->security.ssp_len);
                printf("\tCheck msg permissions: ");
                if (IS_SUCCESS(result)) {
                    printf("trustworthy\n");
                }
                else {
                    printf("untrustworthy\n");
                }
            }

            /* Display the decoded CAM content. */
            printf("[ Received CAM from station %d ]\n", p_rx_decode_cam->header.stationID);
            printf("\tSource LL_ADDR: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   p_recv_ind->src_pv.addr.mid[0], p_recv_ind->src_pv.addr.mid[1],
                   p_recv_ind->src_pv.addr.mid[2], p_recv_ind->src_pv.addr.mid[3],
                   p_recv_ind->src_pv.addr.mid[4], p_recv_ind->src_pv.addr.mid[5]);
            printf("\tStation type: %u\n", p_rx_decode_cam->cam.camParameters.basicContainer.stationType);
            printf("\tgenerationDeltaTime: %u\n", p_rx_decode_cam->cam.generationDeltaTime);
            printf("\tLatitude: %u\n", p_rx_decode_cam->cam.camParameters.basicContainer.referencePosition.latitude);
            printf("\tLongitude: %u\n", p_rx_decode_cam->cam.camParameters.basicContainer.referencePosition.longitude);
            printf("\tAltitude: %u\n", p_rx_decode_cam->cam.camParameters.basicContainer.referencePosition.altitude.altitudeValue);


            if (p_rx_decode_cam->cam.camParameters.basicContainer.stationType == GN_ITS_STATION_SPECIAL_VEHICLE) {
                printf("\tlowFrequencyContainer: %u\n", p_rx_decode_cam->cam.camParameters.lowFrequencyContainer_option);
                if (p_rx_decode_cam->cam.camParameters.lowFrequencyContainer_option) {
                    printf("\tvehicleRole: %u\n", p_rx_decode_cam->cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.vehicleRole);
                    printf("\texteriorLights: %u\n", asn1_bstr_ffs(&p_rx_decode_cam->cam.camParameters.lowFrequencyContainer.u.basicVehicleContainerLowFrequency.exteriorLights));
                }
                printf("\tspecialVehicleContainer: %u\n", p_rx_decode_cam->cam.camParameters.specialVehicleContainer_option);
                if (p_rx_decode_cam->cam.camParameters.specialVehicleContainer_option) {
                    printf("\tlightBarSirenInUse: %u\n", asn1_bstr_ffs(&p_rx_decode_cam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.lightBarSirenInUse));
                    if (p_rx_decode_cam->cam.camParameters.specialVehicleContainer.choice == SpecialVehicleContainer_emergencyContainer) {
                        if (p_rx_decode_cam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication_option) {
                            printf("\tcauseCode: %u\n", p_rx_decode_cam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.causeCode);
                            printf("\tsubCauseCode: %u\n", p_rx_decode_cam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode);
                        }
                        if (p_rx_decode_cam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority_option) {
                            printf("\temergencyPriority: %u\n", asn1_bstr_ffs(&p_rx_decode_cam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority));
                        }
                    }
                }
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
static void *cam_receiver_loop(void *p_param __attribute__((unused)))
{
    uint8_t rx_payload[GN_MAX_SDU_SIZE];
    int32_t result;
    int32_t rx_count = 0;
    btp_handler_recv_indicator_t recv_ind;
    bool ssp_check = false;

    printf("CAM receiver start\n");

    //for (rx_count = 0; rx_count < 10; rx_count++) { // Tushy
    while(rx_count == 0) {
        /* Listen the CAM BTP port. */
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
                cam_decode(rx_payload, result, &recv_ind, ssp_check);
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
static int32_t cam_set_semi_axis_length(double meter)
{
    /*
     * According to ETSI TS 102 894-2 V1.2.1 (2014-09)
     * The value shall be set to:
     * 1 if the accuracy is equal to or less than 1 cm,
     * n (n > 1 and n < 4 093) if the accuracy is equal to or less than n cm,
     * 4 093 if the accuracy is equal to or less than 4 093 cm,
     * 4 094 if the accuracy is out of range, i.e. greater than 4 093 cm,
     * 4 095 if the accuracy information is unavailable.
     */
    double centimeter;
    int32_t value;

    centimeter = meter * 100.0;

    if (centimeter < 1.0) {
        value = 1;
    }
    else if (centimeter > 1.0 && centimeter < 4093.0) {
        value = (int32_t)centimeter;
    }
    else {
        value = 4094;
    }

    return value;
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
static int32_t cam_set_altitude_confidence(double metre)
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

    int32_t enum_value;

    if (metre <= 0.01) {
        enum_value = AltitudeConfidence_alt_000_01;
    }
    else if (metre <= 0.02) {
        enum_value = AltitudeConfidence_alt_000_02;
    }
    else if (metre <= 0.05) {
        enum_value = AltitudeConfidence_alt_000_05;
    }
    else if (metre <= 0.1) {
        enum_value = AltitudeConfidence_alt_000_10;
    }
    else if (metre <= 0.2) {
        enum_value = AltitudeConfidence_alt_000_20;
    }
    else if (metre <= 0.5) {
        enum_value = AltitudeConfidence_alt_000_50;
    }
    else if (metre <= 1.0) {
        enum_value = AltitudeConfidence_alt_001_00;
    }
    else if (metre <= 2.0) {
        enum_value = AltitudeConfidence_alt_002_00;
    }
    else if (metre <= 5.0) {
        enum_value = AltitudeConfidence_alt_005_00;
    }
    else if (metre <= 10.0) {
        enum_value = AltitudeConfidence_alt_010_00;
    }
    else if (metre <= 20.0) {
        enum_value = AltitudeConfidence_alt_020_00;
    }
    else if (metre <= 50.0) {
        enum_value = AltitudeConfidence_alt_050_00;
    }
    else if (metre <= 100.0) {
        enum_value = AltitudeConfidence_alt_100_00;
    }
    else if (metre <= 200.0) {
        enum_value = AltitudeConfidence_alt_200_00;
    }
    else {
        enum_value = AltitudeConfidence_outOfRange;
    }

    return enum_value;
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
static int32_t cam_set_heading_confidence(double degree)
{
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

    int32_t value;

    if (degree <= 0.1) {
        value = 1;
    }
    else if (degree > 1.0 && degree < 125.0) {
        value = (int32_t)(degree * 0.1);
    }
    else {
        value = 126;
    }

    return value;
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
static int32_t cam_set_speed_confidence(double meter_per_sec)
{
    /*
     * According to ETSI TS 102 894-2 V1.2.1 (2014-09)
     * The value shall be set to:
     * 	1 if the speed accuracy is equal to or less than 1 cm/s.
     * 	n (n > 1 and n < 125) if the speed accuracy is equal to or less than n cm/s.
     * 	125 if the speed accuracy is equal to or less than 125 cm/s.
     * 	126 if the speed accuracy is out of range, i.e. greater than 125 cm/s.
     * 	127 if the speed accuracy information is not available.
     */

    double cm_per_sec;
    int32_t value;

    cm_per_sec = meter_per_sec * 100.0;

    if (cm_per_sec <= 1.0) {
        value = 1;
    }
    else if (cm_per_sec > 1.0 && cm_per_sec < 125.0) {
        value = (int32_t)(cm_per_sec);
    }
    else if (cm_per_sec >= 125.0 && cm_per_sec < 126.0) {
        value = 125;
    }
    else {
        value = 126;
    }

    return value;
}

/**
 * function_example - Function cam_check_msg_permission
 *
 * @param   [in]   CAM *p_cam_msg       CAM Message input.
 * @param   [in]   uint8_t *p_ssp       Certificate SSP input.
 * @param   [in]   uint32_t ssp_len     Certificate SSP len input.
 *
 * @return  [int32_t]   Function executing result.
 * @retval  [0]         Success.
 * @retval  [-1]        Fail.
 *
 */
static int32_t cam_check_msg_permission(CAM *p_cam_msg, uint8_t *p_ssp, uint32_t ssp_len)
{
    int32_t rc = 0, fbs;
    if (ssp_len < CAM_SSP_LEN) {
        rc = -1;
        printf("Err: SSP length[%d] is not enough\n", ssp_len);
        goto FAILURE;
    }

    if (p_cam_msg->cam.camParameters.specialVehicleContainer_option) {
        /*
         *   For example, only check emergencyContainer
         *   Please refer to ETSI EN 302 637-2 to check related SSP item
         */
        switch (p_cam_msg->cam.camParameters.specialVehicleContainer.choice) {
            case SpecialVehicleContainer_emergencyContainer:
                if (CAM_SSP_CHECK(EMERGENCY, p_ssp[1]) == false) {
                    printf("Err: certificate not allowed to sign EMERGENCY\n");
                    rc = -1;
                    goto FAILURE;
                }

                if (p_cam_msg->cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority_option) {
                    fbs = asn1_bstr_ffs(&(p_cam_msg->cam.camParameters.specialVehicleContainer.u.emergencyContainer.emergencyPriority));
                    switch (fbs) {
                        case EmergencyPriority_requestForRightOfWay:
                            if (CAM_SSP_CHECK(REQUEST_FOR_RIGHT_OF_WAY, p_ssp[2]) == false) {
                                printf("Err: certificate not allowed to sign REQUEST_FOR_RIGHT_OF_WAY\n");
                                rc = -1;
                                goto FAILURE;
                            }
                            break;
                        case EmergencyPriority_requestForFreeCrossingAtATrafficLight:
                            if (CAM_SSP_CHECK(REQUEST_FOR_FREE_CROSSING_AT_A_TRAFFIC_LIGHT, p_ssp[2]) == false) {
                                printf("Err: certificate not allowed to sign REQUEST_FOR_FREE_CROSSING_AT_A_TRAFFIC_LIGHT\n");
                                rc = -1;
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
    return rc;
}
