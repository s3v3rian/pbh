#ifndef SA_SA_MNGR_H_
#define SA_SA_MNGR_H_

#include "services/gps/gps_poti.h"

#include "sa/processors/its_msg_processor.h"

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

// ---------------------------------------
// ----- ITS MSG PROCESSOR CALLBACKS -----
// ---------------------------------------

typedef bool (*its_processor_init)();
typedef bool (*its_processor_process_tx)(fix_data_t *psPotiFixData);
typedef bool (*its_processor_process_rx_cam)(CAM *psCam, SStationFullFusionData *psLocalStationData, SStationFullFusionData *psRemoteStationData);
typedef bool (*its_processor_process_rx_denm)(DENM *psDenm, SStationFullFusionData *psLocalStationData, SStationFullFusionData *psRemoteStationData);

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

// ---------------------------------------
// ----- ITS MSG PROCESSOR CALLBACKS -----
// ---------------------------------------

its_processor_init g_fp_its_processor_init;
its_processor_process_tx g_fp_its_processor_process_tx;
its_processor_process_rx_cam g_fp_its_processor_proccess_cam;
its_processor_process_rx_denm g_fp_its_processor_proccess_denm;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t sa_mngr_init();
void sa_mngr_release();
void sa_mngr_process_fusion();
void sa_mngr_process_poti();
void sa_mngr_process_cam();
void sa_mngr_process_denm();

#endif


