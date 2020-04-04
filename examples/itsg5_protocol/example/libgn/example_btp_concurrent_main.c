/**
 *******************************************************************************
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential.
 *
 * This example program is created for demonstrating how to perform BTP sending
 * and receiving procedure in 2 different threads (concurrent TX/RX).
 *
 * Usage:
 *      (1) Correct the toolchain setting then type 'make' to build this example program.
 *      (2) Upload the 'example_btp_concurrent_local' and 'example_btp_concurrent_remote'
 *          execution file to 2 different Unex V2X modules.
 *      (3) Run the 'example_btp_concurrent_local' on module A and 'example_btp_concurrent_remote'
 *          on module B, then check whether these 2 devices received BTP packet from each other.
 *
 *       #. Please make sure the "is_mib_its_gn_security_enabled" is disabled in gnd.json,
 *          otherwise the GN daemon will reject any unsecured TX/RX operation.
 *
 * @file    example_btp_concurrent_main.c
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
#include <pthread.h>

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

static btp_handler_ptr btp_handler;


/*
 *******************************************************************************
 * Public functions declaration
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Private functions declaration
 *******************************************************************************
 */

static void *btp_receiver_loop(void *p_param __attribute__((unused)));
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
int32_t main()
{
#ifdef __BTP_REMOTE__
    uint16_t src_port = 5555;
    uint16_t dest_port = 3333;
#else
    uint16_t src_port = 3333;
    uint16_t dest_port = 5555;
#endif  // __BTP_REMOTE__

    uint8_t tx_payload[] = "Hello Unex!";
    uint32_t send_count;
    int32_t result;
    btp_handler_ptr btp_handler;
    btp_handler_config_t btp_config;
    btp_handler_send_config_t send_config;
    pthread_t thread;

    printf("Hello, This is Unex's single hop broadcast(SHB) BTP concurrent example!\n");
    printf("The source port setting is %u!\n", src_port);
    printf("The destination port setting is %u!\n", dest_port);

    /* BTP handler default configuration. */
    BTP_HDLR_CONFIG_INIT(&btp_config, dest_port);

    /* Create BTP handler. */
    result = btp_create(&btp_handler, &btp_config);

    if (IS_SUCCESS(result)) {
        /* Bind BTP port. */
        result = btp_bind(btp_handler, src_port);

        if (IS_SUCCESS(result)) {
            /* Activate BTP receiver thread. */
            pthread_create(&thread, NULL, btp_receiver_loop, NULL);

            /* BTP send packet default configuration (SHB/BTP-B). */
            BTP_SEND_CONFIG_INIT(&send_config);

            /* Send 10 BTP packet. */
            for (send_count = 0; send_count < 10; send_count++) {
                result = btp_send(btp_handler, &send_config, tx_payload, sizeof(tx_payload));

                if (result > 0) {
                    printf("Transmitted %d bytes data\n", result);
                }
                else {
                    printf("Transmitting error: %s\n", ERROR_MSG(result));
                }

                fflush(stdout);
                sleep(1);
            }

            /* Wait till btp_receiver_loop is completed. */
            printf("Wait till btp_receiver_loop is completed\n");
            pthread_join(thread, NULL);
        }
        else {
            printf("Bind error: %s\n", ERROR_MSG(result));
        }

        /* Release BTP handler. */
        btp_release(btp_handler);
    }
    else {
        printf("Create error: %s\n", ERROR_MSG(result));
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
static void *btp_receiver_loop(void *p_param __attribute__((unused)))
{
    uint8_t rx_payload[GN_MAX_SDU_SIZE];
    int32_t result;
    btp_handler_recv_indicator_t recv_ind;

    (void)p_param;

    printf("BTP receiver start\n");

    /* Receive BTP packet. */
    result = btp_recv(btp_handler, &recv_ind, rx_payload, sizeof(rx_payload), BTP_RECV_WAIT_FOREVER);

    if (result > 0) {
        printf("Received %d bytes data\n", result);

        btp_dump_pkt(&recv_ind, rx_payload, result);
    }
    else {
        printf("Receiving error: %s\n", ERROR_MSG(result));

        /* Do error handling if needed. */
    }

    /* Terminate this thread. */
    pthread_exit(NULL);
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
