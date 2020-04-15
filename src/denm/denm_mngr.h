#ifndef DENM_DENM_MNGR_H_
#define DENM_DENM_MNGR_H_

#include "denm_infra.h"
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

static btp_handler_ptr g_pBtpDenmHandler;
static btp_handler_send_config_t g_sBtpDenmSendConfig;

DENM g_sDecodedDenm;
ITSMsgCodecErr g_sEncodeDenmErr;
ITSMsgCodecErr g_sDecodeDenmErr;
uint8_t g_aun8DenmRxPayload[GN_MAX_SDU_SIZE];
btp_handler_recv_indicator_t g_sBtpDenmRecvIndicator;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t denm_mngr_init();
int32_t denm_mngr_process_tx(fix_data_t *psPotiFixData);
int32_t denm_mngr_process_rx();
int32_t denm_mngr_release();

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


