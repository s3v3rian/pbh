#include "nmea_infra.h"

#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "common/globals.h"

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t nmea_get_gga_data(fix_data_t *psPotiFixData, SNmeaGgaData *psGgaData) {

    if(NULL == psPotiFixData
            || NULL == psGgaData) {

        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    psGgaData->m_sUcTime.m_un16Year = psPotiFixData->time.utc.year;
    psGgaData->m_sUcTime.m_un8Month = psPotiFixData->time.utc.mon;
    psGgaData->m_sUcTime.m_un8MonthDay = psPotiFixData->time.utc.mday;
    psGgaData->m_sUcTime.m_un8Hour = psPotiFixData->time.utc.hour;
    psGgaData->m_sUcTime.m_un8Minute = psPotiFixData->time.utc.min;
    psGgaData->m_sUcTime.m_un8Second = psPotiFixData->time.utc.sec;
    psGgaData->m_sUcTime.m_un16Milliseconds = psPotiFixData->time.utc.ms;

    psGgaData->m_dLatitude = nmea_convert_decimal_degress_to_degrees_minutes(psPotiFixData->latitude);
    psGgaData->m_dLongitude = nmea_convert_decimal_degress_to_degrees_minutes(psPotiFixData->longitude);
    psGgaData->m_eGpsFix = NMEA_GPS_QUALITY_FIX_GPS_FIX;

    psGgaData->m_n8NumberOfSatellites = psPotiFixData->num_satellites_used;

    return PROCEDURE_SUCCESSFULL;
}

int32_t nmea_get_rmc_data(fix_data_t *psPotiFixData, SNmeaRmcData *psRmcData) {

    if(NULL == psPotiFixData
            || NULL == psRmcData) {

        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    psRmcData->m_sUcTime.m_un16Year = psPotiFixData->time.utc.year;
    psRmcData->m_sUcTime.m_un8Month = psPotiFixData->time.utc.mon;
    psRmcData->m_sUcTime.m_un8MonthDay = psPotiFixData->time.utc.mday;
    psRmcData->m_sUcTime.m_un8Hour = psPotiFixData->time.utc.hour;
    psRmcData->m_sUcTime.m_un8Minute = psPotiFixData->time.utc.min;
    psRmcData->m_sUcTime.m_un8Second = psPotiFixData->time.utc.sec;
    psRmcData->m_sUcTime.m_un16Milliseconds = psPotiFixData->time.utc.ms;

    psRmcData->m_dLatitude = nmea_convert_decimal_degress_to_degrees_minutes(psPotiFixData->latitude);
    psRmcData->m_dLongitude = nmea_convert_decimal_degress_to_degrees_minutes(psPotiFixData->longitude);
    psRmcData->m_dVelcoityInKnots = psPotiFixData->horizontal_speed * 1.944;

    return PROCEDURE_SUCCESSFULL;
}

int32_t nmea_get_rmc_Data(SNmeaRmcData *psRmcData) {

    if(NULL == psRmcData) {

        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    time_t sTime;
    struct tm *timeinfo;

    time(&sTime);
    timeinfo = localtime(&sTime);

    psRmcData->m_sUcTime.m_un16Year = timeinfo->tm_year;
    psRmcData->m_sUcTime.m_un8Month = timeinfo->tm_mon;
    psRmcData->m_sUcTime.m_un8MonthDay = timeinfo->tm_mday;
    psRmcData->m_sUcTime.m_un8Hour = timeinfo->tm_hour;
    psRmcData->m_sUcTime.m_un8Minute = timeinfo->tm_min;
    psRmcData->m_sUcTime.m_un8Second = timeinfo->tm_sec;
    psRmcData->m_sUcTime.m_un16Milliseconds = 0;

    return PROCEDURE_SUCCESSFULL;
}

int32_t nmea_build_gga_msg(SNmeaGgaData *psGgaData, char *pchOutput) {

    if(NULL == psGgaData
            || NULL == pchOutput) {

        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    int32_t n32SentenceSize = sprintf(pchOutput, "$GPGGA,%02d%02d%02d.%02d,%.3f,N,%.3f,E,1,12,1.0,0.0,M,0.0,M,,*00\n",
                                      psGgaData->m_sUcTime.m_un8Hour,
                                      psGgaData->m_sUcTime.m_un8Minute,
                                      psGgaData->m_sUcTime.m_un8Second,
                                      psGgaData->m_sUcTime.m_un16Milliseconds,
                                      psGgaData->m_dLatitude,
                                      psGgaData->m_dLongitude,
                                      psGgaData->m_n8NumberOfSatellites);

    int8_t n8Crc = nmea_compute_sentence_checksum(pchOutput, n32SentenceSize);

    pchOutput[n32SentenceSize - 2] = nmea_convert_dec_to_hex((n8Crc) & 0x0F);
    pchOutput[n32SentenceSize - 3] = nmea_convert_dec_to_hex((n8Crc >> 4) & 0x0F);

    return n32SentenceSize;
}

int32_t nmea_build_gsa_msg(char *pchOutput) {

    int32_t n32SentenceSize = sprintf(pchOutput, "$GPGSA,A,3,01,02,03,04,05,06,07,08,09,10,11,12,1.0,1.0,1.0,*00\n");

    int8_t n8Crc = nmea_compute_sentence_checksum(pchOutput, n32SentenceSize);

    pchOutput[n32SentenceSize - 2] = nmea_convert_dec_to_hex((n8Crc) & 0x0F);
    pchOutput[n32SentenceSize - 3] = nmea_convert_dec_to_hex((n8Crc >> 4) & 0x0F);

    return 0;
}

int32_t nmea_build_rmc_msg(SNmeaRmcData *psRmcData, char *pchOutput) {

    if(NULL == psRmcData
            || NULL == pchOutput) {

        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    int32_t n32SentenceSize = sprintf(pchOutput, "$GPRMC,%02d%02d%02d.%02d,A,%.3f,N,%.3f,E,%.1f,,%02d%02d%02d,0.0,W,*00\n",
                                      psRmcData->m_sUcTime.m_un8Hour,
                                      psRmcData->m_sUcTime.m_un8Minute,
                                      psRmcData->m_sUcTime.m_un8Second,
                                      psRmcData->m_sUcTime.m_un16Milliseconds,
                                      psRmcData->m_dLatitude,
                                      psRmcData->m_dLongitude,
                                      psRmcData->m_dVelcoityInKnots,
                                      psRmcData->m_sUcTime.m_un8MonthDay,
                                      psRmcData->m_sUcTime.m_un8Month,
                                      psRmcData->m_sUcTime.m_un16Year);

    int8_t n8Crc = nmea_compute_sentence_checksum(pchOutput, n32SentenceSize);

    pchOutput[n32SentenceSize - 2] = nmea_convert_dec_to_hex((n8Crc) & 0x0F);
    pchOutput[n32SentenceSize - 3] = nmea_convert_dec_to_hex((n8Crc >> 4) & 0x0F);

    return n32SentenceSize;
}

double nmea_convert_decimal_degress_to_degrees_minutes(double dValue) {

    double dFrac = dValue - (uint64_t)dValue;
    dFrac = dFrac * 60.0;
    double dNewValue = (uint64_t)dValue;
    dNewValue = dNewValue * 100.0;
    dNewValue = dNewValue + dFrac;

    return dNewValue;
}

static int8_t nmea_compute_sentence_checksum(char *pchSentence, int32_t n32SentenceSize) {

    int8_t n8Crc = 0;

    for(uint32_t un32Index = 1; un32Index < n32SentenceSize - 4; un32Index++) {

        n8Crc ^= pchSentence[un32Index];
    }

    return n8Crc;
}

static char nmea_convert_dec_to_hex(int8_t n8Number) {

    if(9 >= n8Number) {

        return n8Number + 48;

    } else if(15 >= n8Number) {

        return n8Number - 10 + 65;
    }

    return 0;
}
