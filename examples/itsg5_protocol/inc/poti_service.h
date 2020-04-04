/*
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef __POTI_SERVICE_H__
#define __POTI_SERVICE_H__

#include <stdint.h>
#include <math.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FIX_MODE_NA = 0, /* Not available */
    FIX_MODE_NO_FIX = 1, /* No fix */
    FIX_MODE_2D = 2, /* 2D position fix */
    FIX_MODE_3D = 3, /* 3D position fix */
} fix_mode_t;

typedef enum {
    SYSTIME_NOT_SYNC = 0, /* system time not sync. */
    SYSTIME_SYNC_GNSS = 1, /* system time sync. to GNSS */
} systime_status_t;

typedef enum {
    ANTENNA_STATUS_NA = 0, /* No status */
    ANTENNA_STATUS_NORMAL = 1, /* normal */
    ANTENNA_STATUS_BAD = 2, /* open or short */
} antenna_status_t;

typedef enum {
    FIX_STATUS_NA = 0, /* No status */
    FIX_STATUS_GNSS = 1, /* fix from gnss */
    FIX_STATUS_DGNSS = 2 /* fix from gnss with dgnss or rtk */
} fix_status_t;

typedef struct poti_tm_t {
    uint16_t year; /* year, to store the number in 4 decimal digits, such 2018 */
    uint16_t ms; /* milliseconds, [0, 999] */
    uint8_t mon; /* month, [1, 12] */
    uint8_t mday; /* day in a month, [1, 31] */
    uint8_t hour; /* hour, [0, 23] */
    uint8_t min; /* minute, [0, 59] */
    uint8_t sec; /* second, [0, 60] */
} poti_tm_t;

#define POTI_TM_INIT \
    {                \
        .year = 0,   \
        .ms = 0,     \
        .mon = 0,    \
        .mday = 0,   \
        .hour = 0,   \
        .min = 0,    \
        .sec = 0     \
    }

typedef struct fix_time_t {
    poti_tm_t utc;
    double tai_since_2004;
} fix_time_t;

#define FIX_TIME_INIT         \
    {                         \
        .utc = POTI_TM_INIT,  \
        .tai_since_2004 = NAN \
    }

/** Type of GNSS satellite systems */
typedef enum {
    SAT_TYPE_GPS = 0,
    SAT_TYPE_GLONASS = 1,
    SAT_TYPE_NUM
} satellite_type_t;

typedef struct fix_data_t {
    fix_status_t status;
    fix_mode_t mode;
    fix_time_t time;
    /* position */
    double latitude; /* degree, [-90.0, 90.0] */
    double longitude; /* degree, [-180.0, 180.0] */
    double altitude; /* meter */

    /* heading, speed */
    double course_over_ground; /* relative to true north (clockwise) in degree. [0.0, 360.0] */
    double horizontal_speed; /* meter per second */
    double vertical_speed; /* meter per second */

    /* Error */
    double err_time; /* second */
    /* horizontal position standard deviation ellipse */
    double err_smajor_axis; /* semi-major axis length in meter */
    double err_sminor_axis; /* semi-minor axis length in meter */
    double err_smajor_orientation; /* major axis direction relative to true norther (clockwise) in degree. [0.0, 360.0] */
    double err_altitude; /* in meter */
    double err_course_over_ground; /* degree */
    double err_horizontal_speed; /* meter per second */
    double err_vertical_speed; /* meter per second */

    /* skyview */
    /* WARNING: 
     *   Move to gnss_info_t. 
     *   Just keep for build correctly and will be removed in next verions. */
    poti_tm_t skyview_time;
    int num_satellites_used;
    int num_satellites_visible[SAT_TYPE_NUM];

} fix_data_t;

#define FIX_DATA_INIT                  \
    {                                  \
        .status = FIX_STATUS_NA,       \
        .mode = FIX_MODE_NA,           \
        .time = FIX_TIME_INIT,         \
        .latitude = NAN,               \
        .longitude = NAN,              \
        .altitude = NAN,               \
        .course_over_ground = NAN,     \
        .horizontal_speed = NAN,       \
        .vertical_speed = NAN,         \
        .err_smajor_orientation = NAN, \
        .err_smajor_axis = NAN,        \
        .err_sminor_axis = NAN,        \
        .err_altitude = NAN,           \
        .err_course_over_ground = NAN, \
        .err_horizontal_speed = NAN,   \
        .err_vertical_speed = NAN,     \
    }

typedef struct {
    systime_status_t time_sync;
    fix_mode_t fix_mode;
    antenna_status_t antenna_status;
    /* skyview */
    poti_tm_t skyview_time;
    int num_satellites_used;
    int num_satellites_visible[SAT_TYPE_NUM];
} gnss_info_t;

#define GNSS_INFO_INIT                       \
    {                                        \
        .time_sync = SYSTIME_SYNC_GNSS,      \
        .fix_mode = FIX_MODE_NA,             \
        .antenna_status = ANTENNA_STATUS_NA, \
        .skyview_time = POTI_TM_INIT,        \
        .num_satellites_used = 0,            \
        .num_satellites_visible = { 0 }      \
    }

typedef enum {
    POTI_CLOCK_ITS = 0,
} poti_clockid_t;

struct poti_service_t;
typedef struct poti_service_t poti_service_t;

typedef struct poti_config_t {
} poti_config_t;

int poti_create_service(poti_service_t **poti_serv, const poti_config_t *conf);
int poti_get_fix_data(poti_service_t *poti_serv, fix_data_t *fix_data);
int poti_wait_gnss_data(poti_service_t *poti_serv, const struct timespec *ts);
int poti_get_gnss_info(poti_service_t *poti_serv, gnss_info_t *gnss_info);
int poti_clock_gettime(poti_service_t *poti_serv, poti_clockid_t clk_id, struct timespec *tp);
void poti_release_service(poti_service_t *poti_serv);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

static char *fix_mode_str[] = {
    "FIX_MODE_NA", /* no output from GNSS */
    "FIX_MODE_NO_FIX", /* no valid fix from GNSS */
    "FIX_MODE_2D", /* 2D fix */
    "FIX_MODE_3D" /* 3D fix */
};
#define FIX_MODE_TO_STR(mode) (fix_mode_str[(mode)])

static char *systime_status_str[] = {
    "SYSTIME_NOT_SYNC", /* system time not sync. */
    "SYSTIME_SYNC_GNSS" /* system time sync. to GNSS */
};
#define SYSTIME_STATUS_TO_STR(state) (systime_status_str[(state)])

static char *antenna_status_str[] = {
    "ANTENNA_STATUS_NA",
    "ANTENNA_STATUS_NORMAL", /* normal */
    "ANTENNA_STATUS_BAD" /* open or short */
};
#define ANTENNA_STATUS_TO_STR(status) (antenna_status_str[(status)])

#pragma GCC diagnostic pop


#ifdef __cplusplus
}
#endif

#endif /* __POTI_SERVICE_H__ */
