#ifndef SA_SA_MNGR_H_
#define SA_SA_MNGR_H_

#include "services/gps/gps_poti.h"
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

int32_t sa_mngr_init();
void sa_mngr_process_fusion();
void sa_mngr_process_poti();
void sa_mngr_process_cam();
void sa_mngr_process_denm();
void sa_mngr_release();

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

static void sa_mngr_update_db(int32_t n32MsgId, char *pchMsgData);
static void sa_mngr_update_db_cam(CAM *psCam);
static void sa_mngr_update_db_denm(DENM *psDenm);
static void sa_mngr_publish(fix_data_t *psPotiFixData);

static int32_t sa_mngr_output_poti(fix_data_t *psPotiFixData);
static int32_t sa_mngr_output_cam(CAM *psCam);
static int32_t sa_mngr_output_denm(DENM *psDENM);

#endif


