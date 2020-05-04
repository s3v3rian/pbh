#ifndef SA_PROCESSORS_ITS_MSG_PROCESSOR_COMMERICAL_H_
#define SA_PROCESSORS_ITS_MSG_PROCESSOR_COMMERICAL_H_

#include "lib/inc/poti_service.h"
#include "lib/inc/itsmsg_cam.h"
#include "lib/inc/itsmsg_denm.h"

#include "common/globals.h"

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

int32_t its_msg_processor_commercial_init();
int32_t its_msg_processor_commercial_process_tx(fix_data_t *psPotiFixData);
int32_t its_msg_processor_commercial_process_rx_cam(CAM *psCam, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData);
int32_t its_msg_processor_commercial_process_rx_denm(DENM *psDenm, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData);

#endif


