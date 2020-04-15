#ifndef CAM_CAM_MNGR_H_
#define CAM_CAM_MNGR_H_

#include "cam_infra.h"

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

static btp_handler_ptr g_pBtpCamHandler;
static btp_handler_send_config_t g_sBtpCamSendConfig;

CAM g_sDecodedCam;
ITSMsgCodecErr g_sEncodeCamErr;
ITSMsgCodecErr g_sDecodeCamErr;
uint8_t g_aun8CamRxPayload[GN_MAX_SDU_SIZE];
btp_handler_recv_indicator_t g_sBtpCamRecvIndicator;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t cam_mngr_init();
int32_t cam_mngr_process_tx(fix_data_t *psPotiFixData);
int32_t cam_mngr_process_rx();
int32_t cam_mngr_release();

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


