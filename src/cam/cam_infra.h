#ifndef CAM_CAM_INFRA_H_
#define CAM_CAM_INFRA_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "lib/inc/itsmsg.h"
#include "lib/inc/itsmsg_codec.h"
#include "lib/inc/poti_service.h"
#include "lib/inc/gn_service_user.h"
#include "lib/inc/error_code_user.h"

#include "common/globals.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

#define CAM_BTP_PORT                                2001    /* ETSI TS 103 248. */

#define CAM_PROTOCOL_VERSION                        1       /* ETSI EN 302 637-2 V1.3.1 (2014-09) */

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

/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */

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

static CAM cam_tx_encode_fmt;

/*
 *******************************************************************************
 * Public functions declaration
 *******************************************************************************
 */

void cam_infra_init(SITSStationInfo *psStationInfo);
int32_t cam_infra_encode(uint8_t **pp_cam_data, fix_data_t *p_fix_data);
int32_t cam_infra_decode(uint8_t *p_rx_payload, int32_t rx_payload_len, btp_handler_recv_indicator_t *p_recv_ind, bool ssp_check, CAM *psOutputCam, ITSMsgCodecErr *psOutputErr);

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

static int32_t cam_set_semi_axis_length(double meter);
static int32_t cam_set_altitude_confidence(double metre);
static int32_t cam_set_heading_confidence(double degree);
static int32_t cam_set_speed_confidence(double meter_per_sec);

static int32_t cam_check_msg_permission(CAM *p_cam_msg, uint8_t *p_ssp, uint32_t ssp_len);

#endif
