#include "cam_mngr.h"

#include "gps/gps_sim.h"
#include "sa/sa_mngr.h"

int32_t cam_mngr_init() {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    uint16_t un16CamSrcPrt = CAM_BTP_PORT; /* Listen CAM message from remote ITS station. */
    uint16_t un16CamDestPrt = CAM_BTP_PORT; /* Broadcast CAM message to remote ITS station. */

    btp_handler_config_t sBtpCamConfig;

    BTP_HDLR_CONFIG_INIT(&sBtpCamConfig, un16CamDestPrt);

#if (__EN_SECURITY_FEATURE__)

    /*
     * Enable AID security mode.
     * Please make sure all the security configuration in gnd.json is set correctly.
     */
    sBtpCamConfig.security.type = GN_SEC_ENCAP_PKT_BY_AID_PROFILE;
    sBtpCamConfig.security.its_aid = ITS_SEC_CAM_AID;

#endif

    // Create BTP handler.
    n32Result = btp_create(&g_pBtpCamHandler, &sBtpCamConfig);

    if(false == IS_SUCCESS(n32Result)) {

        printf("Cannot create BTP handler: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    /* Bind BTP port. */
    n32Result = btp_bind(g_pBtpCamHandler, un16CamSrcPrt);

    if(false == IS_SUCCESS(n32Result)) {

        printf("Cannot bind BTP handler: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    /* BTP send packet default configuration (SHB/BTP-B). */
    BTP_SEND_CONFIG_INIT(&g_sBtpCamSendConfig);

#if (__EN_SECURITY_FEATURE__)

    /*
    * Assign CAM service specific permissions according to the actual content in payload.
    * Please refer to ETSI TS 103 097, ETSI EN 302 637-2 for more details.
    * Please refer to Unex-APG-ETSI-GN-BTP for more information of build-in certificates
    */

    /* SSP Version control */
    g_sBtpCamSendConfig.security.ssp[0] = 0x0;
    /* Service-specific parameter */
    g_sBtpCamSendConfig.security.ssp[1] = EMERGENCY; /* Emergency container */
    g_sBtpCamSendConfig.security.ssp[2] = REQUEST_FOR_FREE_CROSSING_AT_A_TRAFFIC_LIGHT; /* EmergencyPriority */
    g_sBtpCamSendConfig.security.ssp_len = 3;

#endif

    /* Allocate a buffer for restoring the decode error information if needed. */
    g_sDecodeCamErr.msg_size = 256;
    g_sDecodeCamErr.msg = calloc(1, g_sDecodeCamErr.msg_size);

    /* Allocate a buffer for restoring the decode error information if needed. */
    g_sEncodeCamErr.msg_size = 256;
    g_sEncodeCamErr.msg = calloc(1, g_sEncodeCamErr.msg_size);

    if(NULL == g_sEncodeCamErr.msg
            || NULL == g_sDecodeCamErr.msg) {

        printf("Cannot allocate memory for CAM error message buffer.\n");

        n32Result = PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

#ifdef __GPS_SIMULATOR_ENABLED__

    if(g_sScenarioInfo.m_un32GpSimSyncId == g_sStationInfo.m_un32StationId) {

        g_sScenarioInfo.m_un32GpSimSyncId = 0;
    }

    if(0 == g_sScenarioInfo.m_un32GpSimSyncId) {

        gps_sim_pause_fix_data(BOOLEAN_FALSE);
    }

#endif

    return n32Result;
}

int32_t cam_mngr_process_tx(fix_data_t *psPotiFixData) {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    uint8_t *pun8TxPayload = NULL;
    int32_t n32TxPayloadSize = 0;

#ifdef __GPS_SIMULATOR_ENABLED__

            // Use simulated values if requested.
            gps_sim_update_fix_data(psPotiFixData);
#endif

    cam_infra_init(&g_sStationInfo);

    // Generate CAM message.
    n32TxPayloadSize = cam_infra_encode(&pun8TxPayload, psPotiFixData);

    if(pun8TxPayload == NULL
            || n32TxPayloadSize <= 0) {

        printf("Failed to generate CAM message\n");
        return PROCEDURE_INVALID_SERVICE_TX_ERROR;
    }

    /* Broadcast the CAM message (BTP-B, SHB). */
    n32Result = btp_send(g_pBtpCamHandler, &g_sBtpCamSendConfig, pun8TxPayload, n32TxPayloadSize);

    if(0 >= n32Result) {

        printf("Transmitting error: %s\n", ERROR_MSG(n32Result));
    }

    // Release the allocated message buffer.
    itsmsg_buf_free(pun8TxPayload);

    // Call SA.
    sa_process_poti_data(psPotiFixData);

    return n32Result;
}

int32_t cam_mngr_process_rx() {

    bool bSspCheck = false;

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    /* Listen the CAM BTP port. */
    int32_t n32CamDataSize = btp_recv(g_pBtpCamHandler, &g_sBtpCamRecvIndicator, g_aun8CamRxPayload, sizeof(g_aun8CamRxPayload), BTP_RECV_WAIT_FOREVER);

    if(0 > n32CamDataSize > 0) {

        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    if(true == IS_DECAP_FAIL(g_sBtpCamRecvIndicator.security.status)) {

        printf("\tSecurity status: decapsulation error (%d), the payload content is untrustworthy\n", g_sBtpCamRecvIndicator.security.status);
        return PROCEDURE_SECURITY_ERROR;
    }

    if(ITS_SEC_SUCCESS == g_sBtpCamRecvIndicator.security.status) {

        bSspCheck = true;

        /* Try to decode the received message. */
        n32Result = cam_infra_decode(g_aun8CamRxPayload, n32CamDataSize, &g_sBtpCamRecvIndicator, bSspCheck, &g_sDecodedCam, &g_sDecodeCamErr);

        //cam_print_decoded(&g_sDecodedCam, &recv_ind);

    } else if(g_sBtpCamRecvIndicator.security.status == ITS_SEC_NA) {

        printf("\tSecurity status: no security protection\n");

    } else {

        printf("\tSecurity status: other (%d)\n", g_sBtpCamRecvIndicator.security.status);
    }

    if(0 > n32Result) {

        printf("Unable to decode CAM RX packet: %s\n", g_sDecodeCamErr.msg);

    } else {

#ifdef __GPS_SIMULATOR_ENABLED__

        if(BOOLEAN_TRUE == g_sScenarioInfo.m_n32IsGpsSimEnabled) {

            if(BOOLEAN_TRUE == gps_sim_is_paused()) {

                if(g_sDecodedCam.header.stationID == g_sScenarioInfo.m_un32GpSimSyncId) {

                    gps_sim_pause_fix_data(BOOLEAN_FALSE);
                }
            }
        }
#endif

        sa_process_cam_data(&g_sDecodedCam);
    }

    return n32Result;
}

int32_t cam_mngr_release() {

    btp_release(g_pBtpCamHandler);

    free(g_sDecodeCamErr.msg);

    return PROCEDURE_SUCCESSFULL;
}
