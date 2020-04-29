#ifndef SA_PROCESSORS_RSU_ITS_PROCESSOR_H_
#define SA_PROCESSORS_RSU_ITS_PROCESSOR_H_

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

int32_t its_msg_processor_rsu_init();
int32_t its_msg_processor_rsu_process_cam(CAM *psCam, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData);
int32_t its_msg_processor_rsu_process_denm(DENM *psDenm, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData);
int32_t its_msg_processor_rsu_process_poti_cam(fix_data_t *psPotiFixData, CAM **p2sCam);
int32_t its_msg_processor_rsu_process_poti_denm(fix_data_t *psPotiFixData, DENM **p2sDenm);
int32_t its_msg_processor_rsu_clear_cam();
int32_t its_msg_processor_rsu_clear_denm();

#endif


