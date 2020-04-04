/**
 *******************************************************************************
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * @file    gn_public.h
 * @brief
 * @author  Unex
 *******************************************************************************
 */

#ifndef __GN_PUBLIC_H__
#define __GN_PUBLIC_H__

#include <stdint.h>


/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

/* GN Ether MAC address size. */
#define GN_MAC_ADDR_SIZE 6

/* Maximum size of GN SDU. */
#define GN_MAX_SDU_SIZE 1398


/*
 *******************************************************************************
 * Non-standard
 *******************************************************************************
 */

/* Network layer packet type */
typedef enum gn_svc_network_pkt_type {
    GN_SVC_NETWORK_PKT_SHB,
    GN_SVC_NETWORK_PKT_TSB,
    GN_SVC_NETWORK_PKT_GBC,
    GN_SVC_NETWORK_PKT_GUC,
    GN_SVC_NETWORK_PKT_GAC,
} gn_svc_network_pkt_type_e;


/*
 *******************************************************************************
 * Standard data type definition
 *******************************************************************************
 */

/* Area Type */
typedef enum gn_area_type {
    GN_AREA_TYPE_CIRCLE,
    GN_AREA_TYPE_RECT,
    GN_AREA_TYPE_ELIP,
} gn_area_type_e;

/* ITS-G5 traffic class definition. */
typedef enum gn_its_g5_traffic_class_id {
    GN_ITS_G5_TC_ID_VO = 0, /* Access category = AC_VO, MAX TX power +33 dBM, 6 Mbit/s, for high-priority DENM. */
    GN_ITS_G5_TC_ID_VI = 1, /* Access category = AC_VI, MAX TX power +23 dBM, 6 Mbit/s, for DENM. */
    GN_ITS_G5_TC_ID_BE = 2, /* Access category = AC_BE, MAX TX power +23 dBM, 6 Mbit/s, for CAM. */
    GN_ITS_G5_TC_ID_BK = 3, /* Access category = AC_BK, MAX TX power +23 dBM, 6 Mbit/s, for multihop DENM, other data traffic. */
} gn_its_g5_traffic_class_id_e;

/* The transport layer header type */
typedef enum gn_transport_hdr_type {
    GN_TRANSPORT_HDR_ANY,
    GN_TRANSPORT_HDR_BTP_A,
    GN_TRANSPORT_HDR_BTP_B,
    GN_TRANSPORT_HDR_IPV6,
} gn_transport_hdr_type_e;

/* Life time base definition. */
typedef enum gn_lifttime_base_value {
    GN_LIFETIME_BASE_50MS = 0,
    GN_LIFETIME_BASE_1S,
    GN_LIFETIME_BASE_10S,
    GN_LIFETIME_BASE_100S
} gn_lifttime_base_value_e;

/* ITS-S type */
typedef enum gn_its_station_type {
    GN_ITS_STATION_UNKNOWN = 0,
    GN_ITS_STATION_PEDESTRAIN = 1,
    GN_ITS_STATION_CYCLIST = 2,
    GN_ITS_STATION_MOPED = 3,
    GN_ITS_STATION_MOTORCYCLE = 4,
    GN_ITS_STATION_PASSENGER_CAR = 5,
    GN_ITS_STATION_BUS = 6,
    GN_ITS_STATION_LIGHT_TRUCK = 7,
    GN_ITS_STATION_HEAVY_TRUCK = 8,
    GN_ITS_STATION_TRAILER = 9,
    GN_ITS_STATION_SPECIAL_VEHICLE = 10,
    GN_ITS_STATION_TRAM = 11,
    GN_ITS_STATION_ROAD_SIDE_UNIT = 15,
} gn_its_station_type_e;

/* GN address definition. */
typedef struct gn_addr {
    uint16_t manual : 1; /* This bit allows distinguishing manually configured network address and the initial GeoNetworking address. */
    uint16_t type : 5; /* ITS-S type, to identify the ITS-S type (GN_ITS_STATION_PEDESTRAIN ... GN_ITS_STATION_ROAD_SIDE_UNIT). */
    uint16_t country_code : 10; /* ITS-S country code. */
    uint8_t mid[GN_MAC_ADDR_SIZE]; /* This field represents the LL_ADDR. */

} __attribute__((packed)) gn_addr_t;

/* GN destination area description definition. */
typedef struct gn_area {
    gn_area_type_e area_type; /* GN_AREA_TYPE_CIRCLE, GN_AREA_TYPE_RECT, GN_AREA_TYPE_ELIP. */
    int32_t pos_latitude; /* WGS84, 1/10 micros degrees. */
    int32_t pos_longitude; /* WGS84, 1/10 micros degrees. */
    uint16_t distance_a; /* Distance a of the geometric shape, meters. */
    uint16_t distance_b; /* Distance b of the geometric shape, meters. */
    uint16_t angle; /* Angle of the geometric shape, degrees from North. */
} __attribute__((packed)) gn_area_t;

/* GN long position vector definition. */
typedef struct gn_long_pv {
    gn_addr_t addr; /* The network address for the GeoAdhoc router entity in the ITS-S. */
    uint32_t timestamp; /*
                         * Expresses the time in milliseconds at which the latitude and longitude of
                         * the ITS-S were acquired by the GeoAdhoc router the time is encoded as:
                         *      TST = TST(TAI) mode 2^32
                         * where  TST(TAI) is the number of elapsed TAI milliseconds
                         * since 2004-01-01 00:00:00.000 UTC.
                         */
    int32_t latitude; /* WGS 84 latitude of the GeoAdhoc router reference position, 1/10 micro degree. */
    int32_t longitude; /* WGS 84 longitude of the GeoAdhoc router reference position, 1/10 micro degree. */

    uint16_t pai : 1; /* Position accuracy indicator of the GeoAdhoc router reference position. */
    int16_t speed : 15; /* Speed of the GeoAdhoc router expressed in signed units of 0.01 metre per second. */
    uint16_t heading; /* Heading of the GeoAdhoc router, expressed in unsigned uints of 0.1 degree from North. */
} __attribute__((packed)) gn_long_pv_t;

/* GN lifetime field definition. */
typedef struct gn_lifetime {
    uint8_t multiplier : 6;
    uint8_t base : 2;
} __attribute__((packed)) gn_lifetime_t;

/* GN traffic class field definition. */
typedef struct gn_traffic_class {
    uint8_t tc_scf : 1;
    uint8_t tc_co : 1;
    uint8_t tc_id : 6;
} __attribute__((packed)) gn_traffic_class_t;

/* GN destination address and area structure definition. */
typedef struct gn_dest {
    gn_addr_t addr;
    gn_area_t area;
} __attribute__((packed)) gn_dest_t;

/* GN basic transport header type A definition. */
typedef struct gn_btp_a_hdr {
    uint16_t dport;
    uint16_t sport;
} __attribute__((packed)) gn_btp_a_hdr_t;

/* GN basic transport header type B definition. */
typedef struct gn_btp_b_hdr {
    uint16_t dport;
    uint16_t dport_info;
} __attribute__((packed)) gn_btp_b_hdr_t;


/*
 *******************************************************************************
 * GN network layer header definition
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


/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif  // __GN_PUBLIC_H__
