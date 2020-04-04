/**
 *******************************************************************************
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * @file    gn_service_user.h
 * @brief
 * @author  Unex
 *******************************************************************************
 */

#ifndef __GN_SERVICE_USER_H__
#define __GN_SERVICE_USER_H__

#include <stdint.h>

#include "gn_public.h"
#include "gn_security_user.h"


/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

#define BTP_RECV_WAIT_FOREVER (0xFFFFFFFF)


/*
 *******************************************************************************
 * Macros definition
 *******************************************************************************
 */

/*
 * Macro for reseting GN optional configuration bitmask to all zero.
 */
#define GN_RESET_OPTIONAL_CONFIG(bitmask) \
    do {                                  \
        bitmask = 0x0;                    \
    } while (0)

/*
 * Macro for enabling specific GN optional configuration.
 */
#define GN_ENABLE_OPTIONAL_CONFIG(bitmask, option_bit) \
    do {                                               \
        bitmask |= (option_bit);                       \
    } while (0)

/*
 * Macro for disabling specific GN optional configuration.
 */
#define GN_DISABLE_OPTIONAL_CONFIG(bitmask, option_bit) \
    do {                                                \
        bitmask &= ~(option_bit);                       \
    } while (0)

/*
 * Macro for initializing BTP handler default configuration:
 *      BTP-B, Single hop broadcast, lifetime = 1s, priority = GN_ITS_G5_TC_ID_BK,
 *      TX power 20 dBm, datarate 6 Mbit/s.
 */
#define BTP_HDLR_CONFIG_INIT(btp_config, _dest_port)                               \
    do {                                                                           \
        btp_handler_config_t *p_btp_config = (btp_handler_config_t *)(btp_config); \
        p_btp_config->media.media_selector = (uintptr_t)NULL;                      \
        p_btp_config->media.traffic_class.tc_scf = false;                          \
        p_btp_config->media.traffic_class.tc_co = false;                           \
        p_btp_config->media.traffic_class.tc_id = GN_ITS_G5_TC_ID_BK;              \
        p_btp_config->media.tx_power_dbm = 20;                                     \
        p_btp_config->media.tx_datarate_500kbit_sec = 12;                          \
        p_btp_config->network.type = GN_SVC_NETWORK_PKT_SHB;                       \
        p_btp_config->network.max_hop_limit = 1;                                   \
        p_btp_config->network.max_lifetime.base = GN_LIFETIME_BASE_1S;             \
        p_btp_config->network.max_lifetime.multiplier = 60;                        \
        p_btp_config->network.max_repeat_time = 0;                                 \
        p_btp_config->network.repeat_interval = 0;                                 \
        p_btp_config->transport.type = GN_TRANSPORT_HDR_BTP_B;                     \
        p_btp_config->transport.dest_port = _dest_port;                            \
        p_btp_config->transport.dest_port_info = 0;                                \
        memset((void *)(&p_btp_config->network.dest_addr), 0,                      \
               sizeof(p_btp_config->network.dest_addr));                           \
        memset((void *)(&p_btp_config->network.dest_area), 0,                      \
               sizeof(p_btp_config->network.dest_area));                           \
        p_btp_config->security.type = GN_SEC_ENCAP_PKT_UNSECURE;                   \
        p_btp_config->security.its_aid = ITS_SEC_INVALID_AID;                      \
    } while (0)

/*
 * Marco for initializing BTP send packet default configuration.
 * (Using BTP handler default setting.)
 */
#define BTP_SEND_CONFIG_INIT(send_config)                         \
    do {                                                          \
        btp_handler_send_config_t *p_send_config = (send_config); \
        p_send_config->media_optional.bitmask = 0x00000000;       \
        p_send_config->gn_optional.bitmask = 0x00000000;          \
        p_send_config->btp_optional.bitmask = 0x00000000;         \
        memset((void *)(p_send_config->security.ssp), 0,          \
               sizeof(p_send_config->security.ssp));              \
        p_send_config->security.ssp_len = 0;                      \
    } while (0)

/*
 * Macro for reseting BTP optional configuration bitmask to all zero.
 */
#define BTP_RESET_OPTIONAL_CONFIG(bitmask) \
    do {                                   \
        bitmask = 0x0;                     \
    } while (0)

/*
 * Macro for enabling specific BTP optional configuration.
 */
#define BTP_ENABLE_OPTIONAL_CONFIG(bitmask, option_bit) \
    do {                                                \
        bitmask |= (option_bit);                        \
    } while (0)

/*
 * Macro for disabling specific BTP optional configuration.
 */
#define BTP_DISABLE_OPTIONAL_CONFIG(bitmask, option_bit) \
    do {                                                 \
        bitmask &= ~(option_bit);                        \
    } while (0)


/*
 *******************************************************************************
 * Common data type definition
 *******************************************************************************
 */

/* GN optional configuration bitmask. */
typedef enum gn_option_bitmask {
    GN_OPTION_NETWORK_PKT_TYPE_BIT = (1 << 0),
    GN_OPTION_MAX_HOP_BIT = (1 << 1),
    GN_OPTION_LIFETIME_BIT = (1 << 2),
    GN_OPTION_REPEAT_BIT = (1 << 3),
    GN_OPTION_DEST_ADDR_BIT = (1 << 4),
    GN_OPTION_DEST_AREA_BIT = (1 << 5),
    GN_OPTION_MEDIA_SELECT_BIT = (1 << 6),
    GN_OPTION_TX_PWR_BIT = (1 << 7),
    GN_OPTION_TX_RATE_BIT = (1 << 8),
} gn_option_bitmask_e;

/* BTP optional configuration bitmask. */
typedef enum btp_option_bitmask {
    BTP_OPTION_HDR_TYPE_BIT = (1 << 0),
    BTP_OPTION_DEST_PORT_BIT = (1 << 1),
    BTP_OPTION_DEST_PORT_INFO_BIT = (1 << 2),
} btp_option_bitmask_e;


/*
 *******************************************************************************
 * GN network layer data type definition
 *******************************************************************************
 */

/* GN media layer default configuration. */
typedef struct gn_media_default_config {
    /* To select default communication media or channel. */
    uintptr_t media_selector;

    /*
     * Traffic class configuration:
     * Store-carry-forward,
     * Channel offload,
     * Traffic class ID (GN_ITS_G5_TC_ID_VO, GN_ITS_G5_TC_ID_VI, GN_ITS_G5_TC_ID_VI, GN_ITS_G5_TC_ID_VI)
     */
    gn_traffic_class_t traffic_class;

    /*
     * Default transmit power level [dBm], should not exceed the maximum power
     * limitation which is defined in specified traffic class ID.
     */
    int16_t tx_power_dbm;

    /* Should be 12, 500 Kbits * 12 = 6 Mbit/s by default. */
    uint16_t tx_datarate_500kbit_sec;

} gn_media_default_config_t;

/* GN network layer default configuration. */
typedef struct gn_network_default_config {
    /*
     * Default send packet type (GN_SVC_NETWORK_PKT_SHB, GN_SVC_NETWORK_PKT_TSB,
     *                           GN_SVC_NETWORK_PKT_GBC, GN_SVC_NETWORK_PKT_GUC,
     *                           GN_SVC_NETWORK_PKT_GAC,).
     */
    gn_svc_network_pkt_type_e type;

    /*
     * Maximum life time of TX packet.
     * base: GN_LIFETIME_BASE_50MS, GN_LIFETIME_BASE_1S, GN_LIFETIME_BASE_10S, GN_LIFETIME_BASE_100S.
     */
    gn_lifetime_t max_lifetime;

    /* Maximum HOP limitation of TX packet. */
    uint8_t max_hop_limit;

    /* Repetition interval of TX packet, milliseconds. */
    uint16_t repeat_interval;

    /* Maximum repetition interval of TX packet, milliseconds. */
    uint16_t max_repeat_time;

    /* Default destination address. */
    gn_addr_t dest_addr;

    /* Default destination area. */
    gn_area_t dest_area;

} gn_network_default_config_t;

typedef struct gn_media_optional_config {
    /*
     * Set specific bit to enable specific configuration:
     * GN_OPTION_TX_PWR_BIT = (1 << 7),
     * GN_OPTION_TX_RATE_BIT = (1 << 8),
     */
    gn_option_bitmask_e bitmask;

    /* To select default communication media or channel. */
    uintptr_t media_selector;

    /*
     * Default transmit power level [dBm], should not exceed the maximum power
     * limitation which is defined in specified traffic class ID.
     */
    int16_t tx_power_dbm;

    /* Should be 12, 500 Kbits * 12 = 6 Mbit/s by default. */
    uint16_t tx_datarate_500kbit_sec;

} gn_media_optional_config_t;

/* GN network layer optional configuration. */
typedef struct gn_network_optional_config {
    /*
     * Set specific bit to enable specific configuration:
     * GN_OPTION_NETWORK_PKT_TYPE_BIT = (1 << 0),
     * GN_OPTION_MAX_HOP_BIT = (1 << 1),
     * GN_OPTION_LIFETIME_BIT = (1 << 2),
     * GN_OPTION_REPEAT_BIT = (1 << 3),
     * GN_OPTION_DEST_ADDR_BIT = (1 << 4),
     * GN_OPTION_DEST_AREA_BIT = (1 << 5),
     * GN_OPTION_MEDIA_SELECT_BIT = (1 << 6),
     */
    gn_option_bitmask_e bitmask;

    /*
     * Default send packet type (GN_SVC_NETWORK_PKT_SHB, GN_SVC_NETWORK_PKT_TSB,
	 *                           GN_SVC_NETWORK_PKT_GBC, GN_SVC_NETWORK_PKT_GUC,
	 *                           GN_SVC_NETWORK_PKT_GAC,).
	 */
    gn_svc_network_pkt_type_e type;

    /* Maximum life time of TX packet, milliseconds. */
    gn_lifetime_t max_lifetime;

    /* Maximum HOP limitation of TX packet. */
    uint8_t max_hop_limit;

    /* Repetition interval of TX packet, milliseconds. */
    uint16_t repeat_interval;

    /* Maximum repetition interval of TX packet, milliseconds. */
    uint16_t max_repeat_time;

    /* Default destination address. */
    gn_addr_t dest_addr;

    /* Default destination area. */
    gn_area_t dest_area;

} gn_network_optional_config_t;

typedef struct gn_media_recv_status {
    uint16_t datarate_500kbit_sec;
    int16_t rssi;
} gn_media_recv_status_t;


/*
 *******************************************************************************
 * ITS-G5 management layer data type definition
 *******************************************************************************
 */

typedef intptr_t itsg5_mgnt_handler_t;
typedef intptr_t itsg5_mgnt_handler_ptr;


/*
 *******************************************************************************
 * BTP transport layer data type definition
 *******************************************************************************
 */

/* The public BTP handler data structure. */
typedef intptr_t btp_handler_t;
typedef intptr_t btp_handler_ptr;

/* Configuration of BTP handler. */
typedef struct btp_handler_config {
    /* Media layer configuration. */
    gn_media_default_config_t media;

    /* GN network layer configuration. */
    gn_network_default_config_t network;

    /* Transport layer configuration. */
    struct {
        /* Default BTP header type, (GN_TRANSPORT_HDR_BTP_A, GN_TRANSPORT_HDR_BTP_B) */
        gn_transport_hdr_type_e type;

        /* Default destination port of send packet. */
        uint16_t dest_port;

        /* Destination port info parameter (For BTP-B header only). */
        uint16_t dest_port_info;

    } transport;

    /* Security configuration. */
    struct {
        gn_sec_encap_type_e type; /*
                                   * Set to GN_SEC_ENCAP_PKT_BY_AID_PROFILE for performing the encapsulating procedure according to inputted AID setting,
                                   * Set to GN_SEC_ENCAP_PKT_UNSECURE for encapsulating the packet to unsecured format.
                                   */
        uint32_t its_aid; /* ITS application ID, refer to ETSI TS 103 097. */
    } security;

} btp_handler_config_t;

/* BTP packet transmitting configuration. */
typedef struct btp_handler_send_config {
    /*
     * Optional option,
     *      enable corresponding bit in bitmask for applying unique configuration to this packet,
     *      otherwise the default configuration will be applied automatically.
     */

    /* Optional media layer configuration. */
    gn_media_optional_config_t media_optional;

    /* GN network layer optional configuration. */
    gn_network_optional_config_t gn_optional;

    /* GN transport layer optional configuration. */
    struct {
        /* BTP_OPTION_HDR_TYPE, BTP_OPTION_DEST_PORT_BIT, ... */
        btp_option_bitmask_e bitmask;

        /* Send BTP header type (GN_TRANSPORT_HDR_BTP_A, GN_TRANSPORT_HDR_BTP_B) */
        gn_transport_hdr_type_e type;

        /* Destination port of send packet. */
        uint16_t dest_port;

        /* Destination port info parameter (For BTP-B header only). */
        uint16_t dest_port_info;

    } btp_optional;

    /* Security parameter of TX packet. */
    struct {
        uint32_t ssp_len; /* Actual length of SSP content. */
        uint8_t ssp[GN_SEC_SSP_BUF_SIZE]; /* Service specific permissions, refer to ETSI TS 103 097. */
    } security;

} btp_handler_send_config_t;

/* BTP packet receiving indicator. */
typedef struct btp_handler_recv_indicator {
    /* RSSI, data rate, ... */
    gn_media_recv_status_t media_status;

    gn_svc_network_pkt_type_e network_pkt_type;

    /* Received BTP header type (GN_TRANSPORT_HDR_BTP_A, GN_TRANSPORT_HDR_BTP_B) */
    gn_transport_hdr_type_e transport_pkt_type;

    union {
        gn_btp_a_hdr_t btp_a;
        gn_btp_b_hdr_t btp_b;
    } btp_hdr;

    gn_dest_t destination;
    gn_long_pv_t src_pv;
    gn_traffic_class_t traffic_class;
    gn_lifetime_t remain_lifetime;
    uint8_t remain_hop_limit;

    struct timeval pkt_timestamp;

    /* Security information of RX packet. */
    struct {
        uint32_t its_aid;
        uint32_t ssp_len; /* Actual length of SSP content. */
        uint8_t ssp[GN_SEC_SSP_BUF_SIZE]; /* Service specific permissions, refer to ETSI TS 103 097. */
        isc_decap_status status;
    } security;

} btp_handler_recv_indicator_t;


/*
 *******************************************************************************
 * ITS-G5 management public functions declaration
 *******************************************************************************
 */

/**
 * itsg5_mgnt_create - Function to create a new ITS-G5 management service handler.
 *
 * @param   [in]        *pp_handler     ITS-G5 management handler ID buffer.
 *
 * @return  [int32_t]   Allocated ITS-G5 management handler ID.
 * @retval  [0 ... N]   Valid handler ID.
 * @retval  [-1 ... -N] Function error.
 *
 */
int32_t itsg5_mgnt_create(itsg5_mgnt_handler_ptr *pp_handler);

/**
 * itsg5_mgnt_release - Function to release allocated ITS-G5 management service handler.
 *
 * @param   [in]        p_handler           The ID of allocated ITS-G5 management handler.
 *
 * @return  [int32_t]   Function executing result.
 * @retval  [0]         Success.
 * @retval  [-1 ... -N] Function error.
 *
 */
int32_t itsg5_mgnt_release(itsg5_mgnt_handler_ptr p_handler);

/**
 * itsg5_mgnt_coexistence_mode_set - Function to activate/deactivate ITS-G5 DSRC coexistence mode.
 *
 * @param   [in]        p_handler           The ID of allocated ITS-G5 management handler.
 * @param   [in]        is_enabled          Set true for activating the coexistence mode.
 *
 * @return  [int32_t]   Function executing result.
 * @retval  [0]         Success.
 * @retval  [-1 ... -N] Function error.
 *
 */
int32_t itsg5_mgnt_coexistence_mode_set(itsg5_mgnt_handler_ptr p_handler, bool is_enabled);

/**
 * itsg5_mgnt_coexistence_mode_get - Function to get current ITS-G5 DSRC coexistence mode status.
 *
 * @param   [in]        p_handler           The ID of allocated ITS-G5 management handler.
 * @param   [in]        p_is_enabled        Current coexistence mode status ("true" represents the coexistence mode is activated).
 *
 * @return  [int32_t]   Function executing result.
 * @retval  [0]         Success.
 * @retval  [-1 ... -N] Function error.
 *
 */
int32_t itsg5_mgnt_coexistence_mode_get(itsg5_mgnt_handler_ptr p_handler, bool *p_is_enabled);


/*
 *******************************************************************************
 * BTP public functions declaration
 *******************************************************************************
 */

/**
 * btp_create - Function to create a new BTP service handler.
 *
 * @param   [in]        *pp_handler     BTP handler ID buffer.
 * @param   [in]        *p_btp_config   BTP handler default configuration.
 *
 * @return  [int32_t]   Allocated BTP handler ID.
 * @retval  [0 ... N]   Valid handler ID.
 * @retval  [-1 ... -N] Function error.
 *
 */
int32_t btp_create(btp_handler_ptr *pp_handler, btp_handler_config_t *p_btp_config);

/**
 * btp_release - Function to release allocated BTP handler.
 *
 * @param   [in]        p_handler           The ID of allocated BTP handler.
 *
 * @return  [int32_t]   Function executing result.
 * @retval  [0]         Success.
 * @retval  [-1 ... -N] Function error.
 *
 */
int32_t btp_release(btp_handler_ptr p_handler);

/**
 * btp_bind - Function to bind specific BTP port.
 *
 * @param   [in]        p_handler           The ID of allocated BTP handler.
 * @param   [in]        bind_port           Port number 0 ~ 65535.
 *
 * @return  [int32_t]   Function executing result.
 * @retval  [0]         Success.
 * @retval  [-1 ... -N] Function error.
 *
 */
int32_t btp_bind(btp_handler_ptr p_handler, uint16_t bind_port);

/**
 * btp_send - Function to transmit BTP packet.
 *
 * @param   [in]        p_handler           The ID of allocated BTP handler.
 * @param   [in]        *p_send_config      Configuration of this new packet.
 * @param   [in]        *p_payload          Payload of this new packet.
 * @param   [in]        payload_size        Payload length.
 *
 * @return  [int32_t]   Function executing result.
 * @retval  [1 ... N]   Total transmitted bytes (Payload part only)
 * @retval  [-1 ... -N] Function error.
 *
 */
int32_t btp_send(btp_handler_ptr p_handler, btp_handler_send_config_t *p_send_config,
                 uint8_t *p_payload, uint32_t payload_size);

/**
 * btp_recv - Function to access received BTP packet (which is transmitted to binded port).
 *
 * @param   [in]        p_handler           The ID of allocated BTP handler.
 * @param   [in]        *p_recv_ind         Information of received packet.
 * @param   [in]        *p_payload_buf      Buffer for storing payload of received packet.
 * @param   [in]        payload_buf_size    Buffer size.
 * @param   [in]        timeout_millis      Receiver timeout setting, milliseconds.
 *
 * @return  [int32_t]   Function executing result.
 * @retval  [1 ... N]   Total received bytes (Payload part only)
 * @retval  [-1 ... -N] Function error.
 *
 */
int32_t btp_recv(btp_handler_ptr p_handler, btp_handler_recv_indicator_t *p_recv_ind,
                 uint8_t *p_payload_buf, uint32_t payload_buf_size, uint32_t timeout_millis);


/*
 *******************************************************************************
 * Private functions declaration
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */


#endif  // __GN_SERVICE_USER_H__
