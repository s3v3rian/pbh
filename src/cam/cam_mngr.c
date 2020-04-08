#include "cam_mngr.h"

#include "gps/nmea_infra.h"
#include "boundary/serial_output.h"

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
    n32Result = btp_bind(pBtpCamHandler, un16CamSrcPrt);

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

    return n32Result;
}

int32_t cam_mngr_process_tx(fix_data_t *psPotiFixData) {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    uint8_t *pun8TxPayload = NULL;
    int32_t n32TxPayloadSize = 0;

    // Generate CAM message.
    n32TxPayloadSize = cam_encode(&pun8TxPayload, psPotiFixData);

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

    // Convert POTI info to NMEA data.
    SNmeaGgaData sGgaData;
    SNmeaRmcData sRmcData;
    char achNmeaSentence[300];
    int32_t n32SentenceSize = 0;

    memset(&sGgaData, 0, sizeof(sGgaData));
    memset(&sRmcData, 0, sizeof(sRmcData));

    // Get data from POTI.
    nmea_get_gga_data(psPotiFixData, &sGgaData);
    nmea_get_rmc_data(psPotiFixData, &sRmcData);

    // Build and send NMEA sentence.
    n32SentenceSize = nmea_build_gga_msg(&sGgaData, achNmeaSentence);
    serial_output_write(achNmeaSentence, n32SentenceSize, "T0");

    // Build and send NMEA sentence.
    n32SentenceSize = nmea_build_rmc_msg(&sRmcData, achNmeaSentence);
    serial_output_write(achNmeaSentence, n32SentenceSize, "T0");

    return n32Result;
}

int32_t cam_mngr_process_rx() {

    bool bSspCheck = false;

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    /* Listen the CAM BTP port. */
    n32Result = btp_recv(g_pBtpCamHandler, &g_sBtpCamRecvIndicator, g_aun8CamRxPayload, sizeof(g_aun8CamRxPayload), BTP_RECV_WAIT_FOREVER);

    if(0 > n32Result > 0) {

        printf("Failed to receive cam data\n");
        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    if(true == IS_DECAP_FAIL(g_sBtpCamRecvIndicator.security.status)) {

        printf("\tSecurity status: decapsulation error (%d), the payload content is untrustworthy\n", g_sBtpCamRecvIndicator.security.status);
        return PROCEDURE_SECURITY_ERROR;
    }

    if(ITS_SEC_SUCCESS == g_sBtpCamRecvIndicator.security.status) {

        bSspCheck = true;

        /* Try to decode the received message. */
        cam_decode(g_aun8CamRxPayload, n32Result, &g_sBtpCamRecvIndicator, bSspCheck, &g_sDecodedCam);

        //cam_print_decoded(&g_sDecodedCam, &recv_ind);

    } else if(g_sBtpCamRecvIndicator.security.status == ITS_SEC_NA) {

        printf("\tSecurity status: no security protection\n");

    } else {

        printf("\tSecurity status: other (%d)\n", g_sBtpCamRecvIndicator.security.status);
    }

    SNmeaRmcData sRmcData;
    char achNmeaSentence[300];
    int32_t n32SentenceSize = 0;

    memset(&sRmcData, 0, sizeof(sRmcData));

    nmea_get_rmc_Data(&sRmcData);

    sRmcData.m_dLatitude = g_sDecodedCam.cam.camParameters.basicContainer.referencePosition.latitude;
    sRmcData.m_dLatitude = sRmcData.m_dLatitude / 10000000;
    sRmcData.m_dLatitude = nmea_convert_decimal_degress_to_degrees_minutes(sRmcData.m_dLatitude);

    sRmcData.m_dLongitude = g_sDecodedCam.cam.camParameters.basicContainer.referencePosition.longitude;
    sRmcData.m_dLongitude = sRmcData.m_dLongitude / 10000000;
    sRmcData.m_dLongitude = nmea_convert_decimal_degress_to_degrees_minutes(sRmcData.m_dLongitude);

    sRmcData.m_dVelcoityInKnots = g_sDecodedCam.cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedValue * 1.944;

    n32SentenceSize = nmea_build_rmc_msg(&sRmcData, achNmeaSentence);
    serial_output_write(achNmeaSentence, n32SentenceSize, "T1");

    return PROCEDURE_SUCCESSFULL;
}

int32_t cam_mngr_release() {

    btp_release(g_pBtpCamHandler);

    return PROCEDURE_SUCCESSFULL;
}
