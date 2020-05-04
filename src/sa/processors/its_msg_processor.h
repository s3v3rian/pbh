#ifndef SA_PROCESSORS_ITS_MSG_PROCESSOR_H_
#define SA_PROCESSORS_ITS_MSG_PROCESSOR_H_

#include "lib/inc/poti_service.h"

#include "common/globals.h"

#include "services/cam/cam_mngr.h"
#include "services/denm/denm_mngr.h"

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

int32_t its_msg_processor_process_tx_pending_denms(fix_data_t *psPotiFixData);
int32_t its_msg_processor_process_tx_cam(fix_data_t *psPotiFixData);
int32_t its_msg_processor_process_tx_denm(fix_data_t *psPotiFixData);

int32_t its_msg_processor_allocate_tx_cam_msg(CAM **p2sCam);
int32_t its_msg_processor_allocate_rx_cam_msg(CAM **p2sCam);
int32_t its_msg_processor_allocate_tx_denm_msg(DENM **p2sDenm);
int32_t its_msg_processor_allocate_rx_denm_msg(DENM **p2sDenm);

bool its_msg_processor_push_tx_cam_msg(CAM *psCam);
bool its_msg_processor_pop_tx_cam_msg(CAM **p2sCam);
bool its_msg_processor_push_tx_denm_msg(DENM *psDenm);
bool its_msg_processor_pop_tx_denm_msg(DENM **p2sDenm);

bool its_msg_processor_is_tx_cam_pending();
bool its_msg_processor_is_tx_denm_pending();

#endif
