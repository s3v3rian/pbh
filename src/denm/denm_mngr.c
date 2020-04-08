#include "denm_mngr.h"

int32_t denm_mngr_init() {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    uint16_t un16DenmSrcPrt = DENM_BTP_PORT; /* Listen DENM message from remote ITS station. */
    uint16_t un16DenmDestPrt = DENM_BTP_PORT; /* Broadcast DENM message to remote ITS station. */

    btp_handler_config_t sBtpDenmConfig;

    BTP_HDLR_CONFIG_INIT(&sBtpDenmConfig, un16DenmDestPrt);

#if (__EN_SECURITY_FEATURE__)

    /*
     * Enable AID security mode.
     * Please make sure all the security configuration in gnd.json is set correctly.
     */
    sBtpDenmConfig.security.type = GN_SEC_ENCAP_PKT_BY_AID_PROFILE;
    sBtpDenmConfig.security.its_aid = ITS_SEC_DEN_AID;

#endif

    // Create BTP handler.
    n32Result = btp_create(&g_pBtpDenmHandler, &sBtpDenmConfig);

    if(false == IS_SUCCESS(n32Result)) {

        printf("Cannot create BTP handler: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    /* Bind BTP port. */
    n32Result = btp_bind(g_pBtpDenmHandler, un16DenmSrcPrt);

    if(false == IS_SUCCESS(n32Result)) {

        printf("Cannot bind BTP handler: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    /* BTP send packet default configuration (SHB/BTP-B). */
    BTP_SEND_CONFIG_INIT(&g_sBtpDenmSendConfig);

#if (__EN_SECURITY_FEATURE__)

    /*
    * Assign DENM service specific permissions according to the actual content in payload.
    * Please refer to ETSI TS 103 097, ETSI EN 302 637-2 for more details.
    * Please refer to Unex-APG-ETSI-GN-BTP for more information of build-in certificates
    */

    /* SSP Version control */
    g_sBtpDenmSendConfig.security.ssp[0] = 0;
    /* Service-specific parameter : CauseCodeType */
    g_sBtpDenmSendConfig.security.ssp[1] = 0;
    g_sBtpDenmSendConfig.security.ssp[2] = WRONG_WAY_DRIVING;
    g_sBtpDenmSendConfig.security.ssp[3] = 0;
    g_sBtpDenmSendConfig.security.ssp_len = 4;

#endif

    return n32Result;
}

int32_t denm_mngr_process_tx(fix_data_t *psPotiFixData) {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    uint8_t *pun8TxPayload = NULL;
    int32_t n32TxPayloadSize = 0;

    // Generate DENM message.
    n32TxPayloadSize = denm_encode(&pun8TxPayload, psPotiFixData);

    if(pun8TxPayload == NULL
            || n32TxPayloadSize <= 0) {

        printf("Failed to generate DENM message\n");
        return PROCEDURE_INVALID_SERVICE_TX_ERROR;
    }

    /* BTP send packet default configuration (GBC/BTP-B). */
    BTP_SEND_CONFIG_INIT(&g_sBtpDenmSendConfig);

    /* Must transmit DENM by using GBC mechanism, so set TX network header type to GBC. */
    GN_ENABLE_OPTIONAL_CONFIG(g_sBtpDenmSendConfig.gn_optional.bitmask, GN_OPTION_NETWORK_PKT_TYPE_BIT);
    g_sBtpDenmSendConfig.gn_optional.type = GN_SVC_NETWORK_PKT_GBC;

    /* Setup destination area according to current position. */
    GN_ENABLE_OPTIONAL_CONFIG(g_sBtpDenmSendConfig.gn_optional.bitmask, GN_OPTION_DEST_AREA_BIT);
    g_sBtpDenmSendConfig.gn_optional.dest_area.area_type = GN_AREA_TYPE_RECT; /* GN_AREA_TYPE_CIRCLE, GN_AREA_TYPE_RECT, GN_AREA_TYPE_ELIP. */
    g_sBtpDenmSendConfig.gn_optional.dest_area.pos_latitude = (int32_t)(psPotiFixData->latitude * 10000000.0); /* WGS84, 1/10 micros degrees. */
    g_sBtpDenmSendConfig.gn_optional.dest_area.pos_longitude = (int32_t)(psPotiFixData->longitude * 10000000.0); /* WGS84, 1/10 micros degrees. */
    g_sBtpDenmSendConfig.gn_optional.dest_area.distance_a = 100; /* Distance a of the geometric shape, meters. */
    g_sBtpDenmSendConfig.gn_optional.dest_area.distance_b = 100; /* Distance b of the geometric shape, meters. */
    g_sBtpDenmSendConfig.gn_optional.dest_area.angle = 0; /* Angle of the geometric shape, degrees from North. */

    /* Broadcast the DENM message (BTP-B, SHB). */
    n32Result = btp_send(g_pBtpDenmHandler, &g_sBtpDenmSendConfig, pun8TxPayload, n32TxPayloadSize);

    if(0 >= n32Result) {

        printf("Transmitting error: %s\n", ERROR_MSG(n32Result));
    }

    // Release the allocated message buffer.
    itsmsg_buf_free(pun8TxPayload);

    return n32Result;
}

int32_t denm_mngr_process_rx() {

    bool bSspCheck = false;

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    /* Listen the CAM BTP port. */
    n32Result = btp_recv(g_pBtpDenmHandler, &g_sBtpDenmRecvIndicator, g_aun8DenmRxPayload, sizeof(g_aun8DenmRxPayload), BTP_RECV_WAIT_FOREVER);

    if(0 > n32Result > 0) {

        printf("Failed to receive denm data\n");
        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    if(true == IS_DECAP_FAIL(g_sBtpDenmRecvIndicator.security.status)) {

        printf("\tSecurity status: decapsulation error (%d), the payload content is untrustworthy\n", g_sBtpDenmRecvIndicator.security.status);
        return PROCEDURE_SECURITY_ERROR;
    }

    if(ITS_SEC_SUCCESS == g_sBtpDenmRecvIndicator.security.status) {

        bSspCheck = true;

        /* Try to decode the received message. */
        denm_decode(g_aun8DenmRxPayload, n32Result, &g_sBtpDenmRecvIndicator, bSspCheck);

        //cam_print_decoded(&g_sDecodedCam, &recv_ind);

    } else if(g_sBtpDenmRecvIndicator.security.status == ITS_SEC_NA) {

        printf("\tSecurity status: no security protection\n");

    } else {

        printf("\tSecurity status: other (%d)\n", g_sBtpDenmRecvIndicator.security.status);
    }
}

int32_t denm_mngr_release() {

    btp_release(g_pBtpDenmHandler);

    return PROCEDURE_SUCCESSFULL;
}
