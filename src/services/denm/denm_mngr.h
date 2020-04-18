#ifndef SERVICES_DENM_DENM_MNGR_H_
#define SERVICES_DENM_DENM_MNGR_H_

#include "lib/inc/itsmsg.h"
#include "lib/inc/itsmsg_codec.h"
#include "lib/inc/itsmsg_denm.h"
#include "lib/inc/poti_service.h"
#include "lib/inc/gn_service_user.h"
#include "globals.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

#define DENM_BTP_PORT                               2002    /* ETSI TS 103 248. */

#define DENM_PROTOCOL_VERSION                       1       /* ETSI EN 302 637-3 V1.2.2 (2014-11) */

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

/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */

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

int32_t denm_mngr_init();
int32_t denm_mngr_process_tx(fix_data_t *psPotiFixData, DENM *psOutputDenm);
int32_t denm_mngr_process_rx(DENM *psOutputDenm);
int32_t denm_mngr_release();

DENM *denm_mngr_allocate_encoded_buffer();
DENM *denm_mngr_allocate_decoded_buffer();

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

static int32_t denm_mngr_msg_init(SITSStationInfo *psStationInfo, DENM *psOutputDenm);
static int32_t denm_mngr_msg_encode(uint8_t **p2un8DenmPayload, fix_data_t *psPotiFixData, DENM *psOutputDenm, ITSMsgCodecErr *psOutputErr);
static int32_t denm_mngr_msg_decode(uint8_t *pun8RxPayload, int32_t n32RxPayloadLength, btp_handler_recv_indicator_t *psBtpRecvStatus, bool bSspCheck, DENM *psOutputDenm, ITSMsgCodecErr *psOutputErr);
static int32_t denm_mngr_check_msg_permissions(DENM *psOutputDenm, uint8_t *pun8Ssp, uint32_t un32SspLength);

#endif


