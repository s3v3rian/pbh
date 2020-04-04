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
 *      (2) Upload the 'example_btp_gbc_sender' and 'example_btp_gbc_receiver' execution file to
 *          2 different Unex V2X modules.
 *      (3) Run the 'example_btp_gbc_sender' on module A and 'example_btp_gbc_receiver' on module B,
 *          then check whether the module B received BTP packet from module A.
 *
 *       #. Please make sure the "is_mib_its_gn_security_enabled" is disabled in gnd.json,
 *          otherwise the GN daemon will reject any unsecured TX/RX operation.
 *
 * @file    example_btp_main.c
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

/* For getting navigation information from POTI service. */
#include "poti_service.h"


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

uint8_t tx_payload_pattern[] = "Hello, Unex example_btp_gbc_main.c (GBC/BTP) example";
uint8_t tx_payload_buf[GN_MAX_SDU_SIZE] = {0};


/*
 *******************************************************************************
 * Public functions declaration
 *******************************************************************************
 */

int32_t btp_gbc_sender(uint16_t dest_port, uint16_t src_port, uint32_t repeat_cnt, uint32_t tx_delay_micros, uint32_t tx_payload_size);
int32_t btp_gbc_receiver(uint16_t dest_port, uint16_t src_port, uint32_t repeat_cnt, uint32_t tx_delay_micros, uint32_t tx_payload_size);


/*
 *******************************************************************************
 * Private functions declaration
 *******************************************************************************
 */

static int32_t btp_get_exec_param(int argc, char **argv, uint16_t *p_dest_port, uint16_t *p_src_port,
                                  uint32_t *p_repeat_cnt, uint32_t *p_tx_delay_micros, uint32_t *p_tx_payload_size);
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
    uint32_t repeat_cnt, tx_delay_micros, tx_payload_size;
    printf("Hello, This is Unex's geographically-scoped broadcast(GBC) BTP example!\n");

#ifdef __BTP_RECEIVER__
    dest_port = 5555;
    src_port = 3333; /* Listen port. */
    repeat_cnt = 10; /* Recv 10 packets. */
    tx_delay_micros = 1000000; /* Unused. */
    tx_payload_size = sizeof(tx_payload_pattern); /* Unused. */
    btp_get_exec_param(argc, argv, &dest_port, &src_port, &repeat_cnt, &tx_delay_micros, &tx_payload_size);
    printf("\t<Destination port>: %u\n", dest_port);
    printf("\t<Source port>: %u\n", src_port);
    printf("\t<RX repeat setting>: %u\n", repeat_cnt);

    btp_gbc_receiver(dest_port, src_port, repeat_cnt, tx_delay_micros, tx_payload_size);

#else
    dest_port = 3333; /* Send to. */
    src_port = 5555;
    repeat_cnt = 10; /* Send 10 packets. */
    tx_delay_micros = 1000000; /* Unused. */
    tx_payload_size = sizeof(tx_payload_pattern); /* Give default size. */
    btp_get_exec_param(argc, argv, &dest_port, &src_port, &repeat_cnt, &tx_delay_micros, &tx_payload_size);
    printf("\t<Destination port>: %u\n", dest_port);
    printf("\t<Source port>: %u\n", src_port);
    printf("\t<TX repeat setting>: %u\n", repeat_cnt);
    printf("\t<TX delay (micro second)>: %u\n", tx_delay_micros);
    printf("\t<TX payload size>: %u\n", tx_payload_size);

    btp_gbc_sender(dest_port, src_port, repeat_cnt, tx_delay_micros, tx_payload_size);

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
int32_t btp_gbc_sender(uint16_t dest_port, uint16_t src_port, uint32_t repeat_cnt, uint32_t tx_delay_micros, uint32_t tx_payload_size)
{
    uint32_t send_count;
    int32_t result;
    btp_handler_ptr btp_handler;
    btp_handler_config_t btp_config;
    btp_handler_send_config_t send_config;
    poti_service_t *p_poti_service = NULL;
    fix_data_t poti_fix_data = FIX_DATA_INIT;

    /* unused. */
    (void)src_port;
    (void)tx_delay_micros;

    /* Register POTI service. */
    result = poti_create_service(&p_poti_service, NULL);

    if (IS_SUCCESS(result)) {
        printf("Created POTI service successfully\n");
    }
    else {
        printf("Cannot create POTI service: %s\n", ERROR_MSG(result));

        return -1;
    }

    /* BTP handler default configuration. */
    BTP_HDLR_CONFIG_INIT(&btp_config, dest_port);

    /* Create BTP handler. */
    result = btp_create(&btp_handler, &btp_config);

    if (IS_SUCCESS(result)) {
        /* BTP send packet default configuration (BTP-B). */
        BTP_SEND_CONFIG_INIT(&send_config);

        /* Send 10 BTP packet. */
        for (send_count = 0; send_count < repeat_cnt; send_count++) {
            /* Wait till the navigation is fixed */
            printf("Waiting for GNSS update\n");
            result = poti_wait_gnss_data(p_poti_service, NULL);

            if (IS_SUCCESS(result)) {
                /* Get fix data. */
                result = poti_get_fix_data(p_poti_service, &poti_fix_data);
                if (IS_SUCCESS(result)) {
                    /* Make sure the navigation information is valid. */
                    if (poti_fix_data.status == FIX_STATUS_NA || poti_fix_data.mode == FIX_MODE_NA || poti_fix_data.mode == FIX_MODE_NO_FIX) {
                        printf("Navigation information is invalid\n");
                    }
                    /* Transmit the GBC/BTP-B packet. */
                    else {
                        /* Set TX header type to GBC. */
                        GN_ENABLE_OPTIONAL_CONFIG(send_config.gn_optional.bitmask, GN_OPTION_NETWORK_PKT_TYPE_BIT);
                        send_config.gn_optional.type = GN_SVC_NETWORK_PKT_GBC;

                        /* Set maximum hop limitation to 2 for demonstration. */
                        GN_ENABLE_OPTIONAL_CONFIG(send_config.gn_optional.bitmask, GN_OPTION_MAX_HOP_BIT);
                        send_config.gn_optional.max_hop_limit = 1;

                        /* Setup destination area according to current position. */
                        GN_ENABLE_OPTIONAL_CONFIG(send_config.gn_optional.bitmask, GN_OPTION_DEST_AREA_BIT);
                        send_config.gn_optional.dest_area.area_type = GN_AREA_TYPE_RECT; /* GN_AREA_TYPE_CIRCLE, GN_AREA_TYPE_RECT, GN_AREA_TYPE_ELIP. */
                        send_config.gn_optional.dest_area.pos_latitude = (int32_t)(poti_fix_data.latitude * 10000000.0); /* WGS84, 1/10 micros degrees. */
                        send_config.gn_optional.dest_area.pos_longitude = (int32_t)(poti_fix_data.longitude * 10000000.0); /* WGS84, 1/10 micros degrees. */
                        send_config.gn_optional.dest_area.distance_a = 500; /* Distance a of the geometric shape, meters. */
                        send_config.gn_optional.dest_area.distance_b = 500; /* Distance a of the geometric shape, meters. */
                        send_config.gn_optional.dest_area.angle = 0; /* Angle of the geometric shape, degrees from North. */

                        result = btp_send(btp_handler, &send_config, tx_payload_buf, tx_payload_size);

                        if (result > 0) {
                            printf("Transmitted %d bytes data\n", result);
                        }
                        else {
                            printf("Transmitting error: %s\n", ERROR_MSG(result));
                        }
                    }
                }
            }

            fflush(stdout);
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
int32_t btp_gbc_receiver(uint16_t dest_port, uint16_t src_port, uint32_t repeat_cnt, uint32_t tx_delay_micros, uint32_t tx_payload_size)
{
    uint8_t rx_payload[GN_MAX_SDU_SIZE];
    uint32_t recv_cnt;
    int32_t result;
    btp_handler_ptr btp_handler;
    btp_handler_config_t btp_config;
    btp_handler_recv_indicator_t recv_ind;

    /* unused. */
    (void)tx_delay_micros;
    (void)tx_payload_size;

    /* BTP handler default configuration. */
    BTP_HDLR_CONFIG_INIT(&btp_config, dest_port);

    /* Create BTP handler. */
    result = btp_create(&btp_handler, &btp_config);

    if (IS_SUCCESS(result)) {
        /* Bind BTP port. */
        result = btp_bind(btp_handler, src_port);

        if (IS_SUCCESS(result)) {
            for (recv_cnt = 0; recv_cnt < repeat_cnt; recv_cnt++) {
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

                fflush(stdout);
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
static int32_t btp_get_exec_param(int argc, char **argv, uint16_t *p_dest_port, uint16_t *p_src_port,
                                  uint32_t *p_repeat_cnt, uint32_t *p_tx_delay_micros, uint32_t *p_tx_payload_size)
{
    uint16_t dest_tmp;
    uint16_t src_tmp;
    uint32_t repeat_cnt_tmp;
    uint32_t tx_delay_micros_tmp;
    uint32_t tx_payload_size_tmp;
    uint32_t tmp_idx;

    /* Initialize the content of payload, put predefined pattern first, then put sequence number 0 ~ 255. */
    for (tmp_idx = 0; tmp_idx < GN_MAX_SDU_SIZE; tmp_idx++) {
        if (tmp_idx < sizeof(tx_payload_pattern)) {
            tx_payload_buf[tmp_idx] = tx_payload_pattern[tmp_idx];
        }
        else {
            tx_payload_buf[tmp_idx] = (uint8_t)tmp_idx;
        }
    }

    if (p_dest_port == NULL || p_src_port == NULL || p_repeat_cnt == NULL || p_tx_delay_micros == NULL || p_tx_payload_size == NULL) {
        return -1;
    }

    /* Get the port setting. */
    if (argc >= 3) {
        if ((sscanf(argv[1], "%hu", &dest_tmp) == 1) && (sscanf(argv[2], "%hu", &src_tmp) == 1)) {
            *p_dest_port = dest_tmp;
            *p_src_port = src_tmp;
        }
    }

    /* Get the repeat TX/RX setting. */
    if (argc >= 4) {
        if ((sscanf(argv[3], "%u", &repeat_cnt_tmp) == 1)) {
            *p_repeat_cnt = repeat_cnt_tmp;
        }
    }

    /* Get the TX delay and payload size setting. */
    if (argc >= 6) {
        if ((sscanf(argv[4], "%u", &tx_delay_micros_tmp) == 1) && (sscanf(argv[5], "%u", &tx_payload_size_tmp) == 1)) {
            *p_tx_delay_micros = tx_delay_micros_tmp;
            *p_tx_payload_size = tx_payload_size_tmp;
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
    uint32_t time_millis;

    if (p_recv_ind == NULL || p_payload == NULL || payload_size == 0) {
        printf("Incorrect input\n");

        return -1;
    }

    nowtime = p_recv_ind->pkt_timestamp.tv_sec;
    time_millis = (uint32_t)round(((double)p_recv_ind->pkt_timestamp.tv_usec) / 1000.0);
    now_tm = localtime(&nowtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", now_tm);

    printf("BTP RX information:\n");
    printf("\tTimestamp: %s.%03u\n", time_str, time_millis);
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
