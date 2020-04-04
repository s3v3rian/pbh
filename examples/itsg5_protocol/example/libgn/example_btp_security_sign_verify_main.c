/**
 *******************************************************************************
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * This example program is created for demonstrating how to perform BTP sending
 * or receiving procedure in one single process. For concurrent TX/RX purpose,
 * please refer to example_btp_concurrent_main.c.
 *
 * Usage:
 *      (1) Correct the toolchain setting then type 'make' to build this example program.
 *      (2) Upload the 'example_btp_sign_sender' and 'example_btp_verify_receiver' execution
 *          file to 2 different Unex V2X modules.
 *      (3) Check and add necessary the security configuration in gnd.json:
 *          "security": {
 *              "is_mib_its_gn_security_enabled": true,
 *              "mib_its_gn_decap_result_handling": 1,
 *              "aid_profile": [
 *                  {
 *                      "aid": 36,
 *                      "profile": "ISC_PROFILE_SIGN"
 *                  },
 *              ]
 *          }
 *      (4) Run the 'example_btp_sign_sender' on module A and 'example_btp_verify_receiver'
 *          on module B, then check whether the module B received BTP packet from module A.
 *
 * @file    example_btp_security_sign_verify_main.c
 * @brief
 * @author  Unex
 *******************************************************************************
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "gn_service_user.h"
#include "error_code_user.h"


/*
 *******************************************************************************
 * Constant value definition
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
 * Public functions declaration
 *******************************************************************************
 */

int32_t btp_sender(uint16_t dest_port, uint16_t src_port);
int32_t btp_receiver(uint16_t dest_port, uint16_t src_port);


/*
 *******************************************************************************
 * Private functions declaration
 *******************************************************************************
 */

static int32_t btp_get_port_param(int argc, char **argv, uint16_t *p_dest_port, uint16_t *p_src_port);
static int32_t btp_dump_pkt(btp_handler_recv_indicator_t *p_recv_ind, uint8_t *p_payload, uint32_t payload_size);
static void btp_dump_certificate_ssp(uint8_t *p_ssp, uint32_t ssp_len);


/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
int32_t main(int argc, char *argv[])
{
    uint16_t dest_port, src_port;
    printf("Hello, This is Unex's single hop broadcast(SHB) BTP security example!\n");
    printf("Please make sure the its_gn_security option is enabled in GN daemon configuration file.\n");
    printf("Please make sure the AID/profile mapping is existed in GN daemon configuration file.\n");
    printf("The recommended test setting is \"aid\": 36, \"profile_name\": \"ISC_PROFILE_SIGN\".\n");

#ifdef __BTP_RECEIVER__
    dest_port = 5555;
    src_port = 3333; /* Listen port. */
    btp_get_port_param(argc, argv, &dest_port, &src_port);
    printf("\t<Destination port>: %u\n", dest_port);
    printf("\t<Source port>: %u\n", src_port);

    btp_receiver(dest_port, src_port);

#else
    dest_port = 3333; /* Send to. */
    src_port = 5555;
    btp_get_port_param(argc, argv, &dest_port, &src_port);
    printf("\t<Destination port>: %u\n", dest_port);
    printf("\t<Source port>: %u\n", src_port);

    btp_sender(dest_port, src_port);

#endif

    return 0;
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
int32_t btp_sender(uint16_t dest_port, uint16_t src_port)
{
    uint8_t tx_payload[] = "Hello Unex (Security sign and verify)!";
    uint32_t send_count;
    int32_t result;
    btp_handler_ptr btp_handler;
    btp_handler_config_t btp_config;
    btp_handler_send_config_t send_config;

    /* unused. */
    (void)src_port;

    /* BTP handler default configuration. */
    BTP_HDLR_CONFIG_INIT(&btp_config, dest_port);

    /*
     * Enable AID security mode.
     * Please make sure all the security configurations in gnd.json are set correctly.
     */
    btp_config.security.type = GN_SEC_ENCAP_PKT_BY_AID_PROFILE;
    btp_config.security.its_aid = ITS_SEC_CAM_AID; /* Using CAM AID for example. */

    /* Create BTP handler. */
    result = btp_create(&btp_handler, &btp_config);

    if (IS_SUCCESS(result)) {
        /* BTP send packet default configuration (SHB/BTP-B). */
        BTP_SEND_CONFIG_INIT(&send_config);

        /*
         * Assign service specific permissions according to the actual content in payload.
         * Please refer to ETSI TS 103 097, ETSI EN 302 637-2, ETSI EN 302 637-3 for more details.
         *
         * Please refer to Unex-APG-ETSI-GN-BTP for more information of build-in certificates
         * Using CAM SSP for example:
         */
        /* SSP Version control */
        send_config.security.ssp[0] = 0x00;
        /* Service-specific parameter */
        send_config.security.ssp[1] = (1 << 1); /* Using CAM, Emergency event for example. */
        send_config.security.ssp[2] = 0x00;
        send_config.security.ssp_len = 3;

        /* Send 10 BTP packet. */
        for (send_count = 0; send_count < 10; send_count++) {
            result = btp_send(btp_handler, &send_config, tx_payload, sizeof(tx_payload));

            if (result > 0) {
                printf("Transmitted %d bytes data (using AID: %u)\n", result, btp_config.security.its_aid);
            }
            else {
                printf("Transmitting error: %s\n", ERROR_MSG(result));
            }

            fflush(stdout);
            sleep(1);
        }

        /* Release BTP handler. */
        btp_release(btp_handler);
    }
    else {
        printf("Sender error: %s\n", ERROR_MSG(result));
    }

    return 0;
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
int32_t btp_receiver(uint16_t dest_port, uint16_t src_port)
{
    uint8_t rx_payload[GN_MAX_SDU_SIZE];
    int32_t result;
    btp_handler_ptr btp_handler;
    btp_handler_config_t btp_config;
    btp_handler_recv_indicator_t recv_ind;

    /* BTP handler default configuration. */
    BTP_HDLR_CONFIG_INIT(&btp_config, dest_port);

    /*
     * Enable AID security mode.
     * Please make sure all the security configurations in gnd.json are set correctly.
     */
    btp_config.security.type = GN_SEC_ENCAP_PKT_BY_AID_PROFILE;
    btp_config.security.its_aid = ITS_SEC_CAM_AID; /* Must always assign a AID for encapsulation. */

    /* Create BTP handler. */
    result = btp_create(&btp_handler, &btp_config);

    if (IS_SUCCESS(result)) {
        /* Bind BTP port. */
        result = btp_bind(btp_handler, src_port);

        if (IS_SUCCESS(result)) {
            /* Receive BTP packet. */
            result = btp_recv(btp_handler, &recv_ind, rx_payload, sizeof(rx_payload), BTP_RECV_WAIT_FOREVER);

            if (result > 0) {
                printf("Received %d bytes data\n", result);

                btp_dump_pkt(&recv_ind, rx_payload, result);
            }
            else {
                printf("Receiving error: %s\n", ERROR_MSG(result));

                /* Do error handling if needed. */
                sleep(1);
            }
        }
        else {
            printf("Bind error: %s\n", ERROR_MSG(result));
        }

        /* Release BTP handler. */
        btp_release(btp_handler);
    }
    else {
        printf("Receiver error: %s\n", ERROR_MSG(result));
    }

    return 0;
}


/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static int32_t btp_get_port_param(int argc, char **argv, uint16_t *p_dest_port, uint16_t *p_src_port)
{
    uint16_t dest_tmp;
    uint16_t src_tmp;


    if (p_dest_port == NULL || p_src_port == NULL)
        return -1;

    if (argc == 3) {
        if ((sscanf(argv[1], "%hu", &dest_tmp) == 1) && (sscanf(argv[2], "%hu", &src_tmp) == 1)) {
            *p_dest_port = dest_tmp;
            *p_src_port = src_tmp;
        }
    }

    return 0;
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static int32_t btp_dump_pkt(btp_handler_recv_indicator_t *p_recv_ind, uint8_t *p_payload, uint32_t payload_size)
{
    uint32_t idx;
    time_t nowtime;
    struct tm *now_tm;
    char time_str[64];

    if (p_recv_ind == NULL || p_payload == NULL || payload_size == 0) {
        printf("Incorrect input\n");

        return -1;
    }

    nowtime = p_recv_ind->pkt_timestamp.tv_sec;
    now_tm = localtime(&nowtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);

    printf("BTP RX information:\n");
    printf("\tTimestamp: %s\n", time_str);
    printf("\tSource LL_ADDR: %02X:%02X:%02X:%02X:%02X:%02X\n",
           p_recv_ind->src_pv.addr.mid[0], p_recv_ind->src_pv.addr.mid[1],
           p_recv_ind->src_pv.addr.mid[2], p_recv_ind->src_pv.addr.mid[3],
           p_recv_ind->src_pv.addr.mid[4], p_recv_ind->src_pv.addr.mid[5]);
    printf("\tDatarate(500Kbit/s): %u\n", p_recv_ind->media_status.datarate_500kbit_sec);
    printf("\tRSSI(dBm): %d\n", p_recv_ind->media_status.rssi);
    printf("\tNetwork header type: %u\n", p_recv_ind->network_pkt_type);
    printf("\tTransport header type: %u\n", p_recv_ind->transport_pkt_type);

    if (p_recv_ind->network_pkt_type == GN_SVC_NETWORK_PKT_GBC) {
        printf("\tDestination area: type = %d, LAT = %d, LONG = %d, DIST_A = %u, DIST_B = %u, angle = %u\n",
               p_recv_ind->destination.area.area_type, /* GN_AREA_TYPE_CIRCLE, GN_AREA_TYPE_RECT, GN_AREA_TYPE_ELIP. */
               p_recv_ind->destination.area.pos_latitude, /* WGS84, 1/10 micros degrees. */
               p_recv_ind->destination.area.pos_longitude, /* WGS84, 1/10 micros degrees. */
               p_recv_ind->destination.area.distance_a, /* Distance a of the geometric shape, meters. */
               p_recv_ind->destination.area.distance_b, /* Distance b of the geometric shape, meters. */
               p_recv_ind->destination.area.angle); /* Angle of the geometric shape, degrees from North. */
    }

    /* Show RX packet security decapsulation status. */
    if (IS_DECAP_FAIL(p_recv_ind->security.status) == true) {
        printf("\tSecurity status: decapsulation error (%d), the payload content is untrustworthy\n", p_recv_ind->security.status);
    }
    else {
        if (p_recv_ind->security.status == ITS_SEC_SUCCESS) {
            printf("\tSecurity status: this packet is decapsulated with no error\n");
            printf("\tITS-AID: %u\n", p_recv_ind->security.its_aid);
            btp_dump_certificate_ssp(p_recv_ind->security.ssp, p_recv_ind->security.ssp_len);
        }
        else if (p_recv_ind->security.status == ITS_SEC_NA) {
            printf("\tSecurity status: no security protection\n");
        }
        else {
            printf("\tSecurity status: other (%d)\n", p_recv_ind->security.status);
        }
    }

    printf("\tPayload content (not includes network and transport layer headers) <HEX>:");

    for (idx = 0; idx < payload_size; idx++) {
        if (idx % 32 == 0) {
            printf("\n\t\t");
        }

        printf("%02X ", p_payload[idx]);
    }

    printf("\n");

    return 0;
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static void btp_dump_certificate_ssp(uint8_t *p_ssp, uint32_t ssp_len)
{
    uint32_t idx;

    printf("\tSSP setting in certificate <HEX>: ");

    if (ssp_len > ITS_SSP_MAX_LEN) {
        printf("(error: incorrect SSP length)");
    }
    else if (ssp_len == 0) {
        printf("(none: SSP length is zero)");
    }
    else {
        for (idx = 0; idx < ssp_len; idx++) {
            printf("%02X ", p_ssp[idx]);
        }
    }

    printf("\n");
}
