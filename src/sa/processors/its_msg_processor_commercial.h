#ifndef SA_PROCESSORS_COMMERICAL_ITS_PROCESSOR_H_
#define SA_PROCESSORS_COMMERICAL_ITS_PROCESSOR_H_

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
int32_t its_msg_processor_cm_process_cam(CAM *psCam, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData);
int32_t its_msg_processor_cm_process_denm(DENM *psDenm, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData);
int32_t its_msg_processor_cm_process_poti_cam(fix_data_t *psPotiFixData, CAM **p2sCam);
int32_t its_msg_processor_cm_process_poti_denm(fix_data_t *psPotiFixData, DENM **p2sDenm);
int32_t its_msg_processor_cm_clear_cam();
int32_t its_msg_processor_cm_clear_denm();

#endif


