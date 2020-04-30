#ifndef SA_PROCESSORS_ITS_MSG_PROCESSOR_H_
#define SA_PROCESSORS_ITS_MSG_PROCESSOR_H_

#include "lib/inc/poti_service.h"
#include "lib/inc/itsmsg_cam.h"
#include "lib/inc/itsmsg_denm.h"

#include "globals.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */

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

int32_t its_msg_processor_init();

int32_t its_msg_processor_get_tx_cam_msg(CAM **p2sCam);
int32_t its_msg_processor_get_tx_denm_msg(DENM **p2sDenm);
int32_t its_msg_processor_get_rx_cam_msg(CAM **p2sCam);
int32_t its_msg_processor_get_rx_denm_msg(DENM **p2sDenm);

void its_msg_processor_push_cam_msg(CAM *psCam);
void its_msg_processor_push_denm_msg(DENM *psDenm);
void its_msg_processor_pop_cam_msg(CAM **p2sCam);
void its_msg_processor_pop_denm_msg(DENM **p2sDenm);

#endif


