#ifndef CAM_CAM_MNGR_H_
#define CAM_CAM_MNGR_H_

#include "lib/inc/itsmsg.h"
#include "lib/inc/itsmsg_codec.h"
#include "lib/inc/itsmsg_cam.h"
#include "lib/inc/poti_service.h"
#include "lib/inc/gn_service_user.h"
#include "globals.h"

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

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t cam_mngr_init();
int32_t cam_mngr_process_tx(fix_data_t *psPotiFixData);
int32_t cam_mngr_process_rx(CAM **p2sOutputCam);
int32_t cam_mngr_release();

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

static int32_t cam_mngr_msg_init(SITSStationInfo *psStationInfo, CAM *psOutputCam);
static int32_t cam_mngr_msg_encode(uint8_t **p2un8CamPayload, fix_data_t *psPotiFixData, CAM *psOutputCam, ITSMsgCodecErr *psOutputErr);
static int32_t cam_mngr_msg_decode(uint8_t *pun8RxPayload, int32_t n32RxPayloadLength, btp_handler_recv_indicator_t *psBtpRecvStatus, bool bSspCheck, CAM *psOutputCam, ITSMsgCodecErr *psOutputErr);

static int32_t cam_mngr_set_semi_axis_length(double dMeter);
static int32_t cam_mngr_set_altitude_confidence(double dMeter);
static int32_t cam_mngr_set_heading_confidence(double dDegree);
static int32_t cam_mngr_set_speed_confidence(double dMeterPerSec);
static int32_t cam_mngr_check_msg_permissions(CAM *psOutputCam, uint8_t *pun8Ssp, uint32_t un32SspLength);

#endif


