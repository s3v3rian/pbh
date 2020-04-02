#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "poti_service.h"
#include "error_code_user.h"

char *mode_str[] = {"NA", "NO_FIX", "2D_FIX", "3D_FIX"};

void print_error(fix_data_t *fix_data)
{
    if (!isnan(fix_data->err_time)) {
        printf("err_time: %lf ", fix_data->err_time);
    }
    if (!isnan(fix_data->err_smajor_axis)) {
        printf("err_smajor_axis: %lf ", fix_data->err_smajor_axis);
    }
    if (!isnan(fix_data->err_sminor_axis)) {
        printf("err_sminor_axis: %lf ", fix_data->err_sminor_axis);
    }
    if (!isnan(fix_data->err_smajor_orientation)) {
        printf("err_smajor_orientation: %lf ", fix_data->err_smajor_orientation);
    }
    if (!isnan(fix_data->err_altitude)) {
        printf("err_altitude: %lf ", fix_data->err_altitude);
    }
    if (!isnan(fix_data->err_course_over_ground)) {
        printf("err_course_over_ground: %lf ", fix_data->err_course_over_ground);
    }
    if (!isnan(fix_data->err_horizontal_speed)) {
        printf("err_horizontal_speed: %lf ", fix_data->err_horizontal_speed);
    }
    if (!isnan(fix_data->err_vertical_speed)) {
        printf("err_vertical_speed: %lf ", fix_data->err_vertical_speed);
    }
    printf("\n");
}

void print_skyview(gnss_info_t *info)
{
    int i, visible = 0;
    for (i = 0; i < SAT_TYPE_NUM; i++) {
        visible += info->num_satellites_visible[i];
    }
    printf("[Skyview] %d/%d @ %d/%02d/%02d %02d:%02d:%02d.%03d\n",
           info->num_satellites_used, visible,
           info->skyview_time.year, info->skyview_time.mon, info->skyview_time.mday, info->skyview_time.hour, info->skyview_time.min, info->skyview_time.sec, info->skyview_time.ms);
}

void print_status_info(gnss_info_t *info)
{
    printf("[STATUS] Antenna: %s, Mode: %s, SYS Time: %s\n",
           ANTENNA_STATUS_TO_STR(info->antenna_status),
           FIX_MODE_TO_STR(info->fix_mode),
           SYSTIME_STATUS_TO_STR(info->time_sync));
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    int rc;
    poti_service_t *serv;
    fix_data_t fix_data = FIX_DATA_INIT;
    gnss_info_t gnss_info = GNSS_INFO_INIT;
    struct timespec tp;
    /* the value of timeout is based on 10Hz GNSS */
    struct timespec timeout = {.tv_sec = 0, .tv_nsec = 150000000};

    rc = poti_create_service(&serv, NULL);
    if (rc != 0) {
        printf("Error, poti: %s\n", error_msg(rc));
        exit(1);
    }

    rc = poti_get_gnss_info(serv, &gnss_info);
    if (rc != 0) {
        printf("Error, poti_get_gnss_info: %s\n", error_msg(rc));
    }
    else {
        print_status_info(&gnss_info);
    }

    while (1) {
        printf("=======================================\n");
        rc = poti_wait_gnss_data(serv, &timeout);

        if (rc != 0) {
            printf("Error, poti_wait_gnss_data: %s\n", error_msg(rc));

            rc = poti_get_gnss_info(serv, &gnss_info);
            if (rc != 0) {
                printf("Error, poti_get_gnss_info: %s\n", error_msg(rc));
            }
            else {
                print_status_info(&gnss_info);
            }
        }
        else {
            rc = poti_get_fix_data(serv, &fix_data);
            if (rc != 0) {
                printf("Error, poti_get_fix_data: %s\n", error_msg(rc));
                continue;
            }

            rc = poti_clock_gettime(serv, POTI_CLOCK_ITS, &tp);
            if (rc == 0) {
                printf("[system clock] tai_since_2004: %ld.%09ld\n", tp.tv_sec, tp.tv_nsec);
            }
            if (fix_data.mode == FIX_MODE_2D) {
                printf("[2D Fix] %d/%02d/%02d %02d:%02d:%02d.%03d (%lf) | %lf, %lf\n",
                       fix_data.time.utc.year, fix_data.time.utc.mon, fix_data.time.utc.mday, fix_data.time.utc.hour, fix_data.time.utc.min, fix_data.time.utc.sec, fix_data.time.utc.ms,
                       fix_data.time.tai_since_2004,
                       fix_data.latitude, fix_data.longitude);
                printf("\t");
                printf("heading: %lf, speed: %lf\n", fix_data.course_over_ground, fix_data.horizontal_speed);
                printf("\t");
                print_error(&fix_data);
            }
            else if (fix_data.mode == FIX_MODE_3D) {
                printf("[3D Fix] %d/%02d/%02d %02d:%02d:%02d.%03d (%lf) | %lf, %lf (%lfm)\n",
                       fix_data.time.utc.year, fix_data.time.utc.mon, fix_data.time.utc.mday, fix_data.time.utc.hour, fix_data.time.utc.min, fix_data.time.utc.sec, fix_data.time.utc.ms,
                       fix_data.time.tai_since_2004,
                       fix_data.latitude, fix_data.longitude, fix_data.altitude);
                printf("\t");
                printf("heading: %lf, speed: %lf, vertical speed: %lf\n", fix_data.course_over_ground, fix_data.horizontal_speed, fix_data.vertical_speed);
                printf("\t");
                print_error(&fix_data);
            }
            else {
                printf("Mode: %s \n", mode_str[fix_data.mode]);
            }

            rc = poti_get_gnss_info(serv, &gnss_info);
            if (rc != 0) {
                printf("Error, poti_get_gnss_info: %s\n", error_msg(rc));
            }
            else {
                print_skyview(&gnss_info);
            }
        }
        fflush(stdout);
    }

    poti_release_service(serv);

    return 0;
}
