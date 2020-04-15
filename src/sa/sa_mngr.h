#ifndef SA_SA_MNGR_H_
#define SA_SA_MNGR_H_

#include "cam/cam_infra.h"
#include "denm/denm_infra.h"

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

void sa_process_services();
int32_t sa_process_poti_data(fix_data_t *psPotiFixData);
int32_t sa_process_cam_data(CAM *psCam);
int32_t sa_process_denm_data(DENM *psDenm);

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


