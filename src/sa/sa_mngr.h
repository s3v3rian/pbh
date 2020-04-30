#ifndef SA_SA_MNGR_H_
#define SA_SA_MNGR_H_

#include "services/gps/gps_poti.h"
#include "services/cam/cam_mngr.h"
#include "services/denm/denm_mngr.h"

#include "sa/sa_database.h"

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

typedef int32_t (*its_processor_init)();
typedef int32_t (*its_processor_process_poti)(fix_data_t *psPotiFixData);
typedef int32_t (*its_processor_process_rx_cam)(CAM *psCam, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData);
typedef int32_t (*its_processor_process_rx_denm)(DENM *psDenm, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData);

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

// ---------------------------------------
// ----- ITS MSG PROCESSOR CALLBACKS -----
// ---------------------------------------

its_processor_init g_fp_its_processor_init;
its_processor_process_poti g_fp_its_processor_process_poti;
its_processor_process_rx_cam g_fp_its_processor_proccess_cam;
its_processor_process_rx_denm g_fp_its_processor_proccess_denm;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t sa_mngr_init();
void sa_mngr_process_fusion();
void sa_mngr_process_poti();
void sa_mngr_process_cam();
void sa_mngr_process_denm();
void sa_mngr_release();

#endif


