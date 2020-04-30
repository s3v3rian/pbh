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

int32_t its_msg_processor_cm_init();
int32_t its_msg_processor_cm_process_poti(fix_data_t *psPotiFixData);
int32_t its_msg_processor_cm_process_rx_cam(CAM *psCam, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData);
int32_t its_msg_processor_cm_process_rx_denm(DENM *psDenm, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData);

#endif


