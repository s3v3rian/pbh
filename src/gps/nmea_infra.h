#ifndef GPS_NMEA_INFRA_H_
#define GPS_NMEA_INFRA_H_

#include <stdint.h>
#include "lib/inc/poti_service.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

enum EN_NMEA_MESSAGE {

        NMEA_MESSAGE_GSA,
	NMEA_MESSAGE_GGA,
        NMEA_MESSAGE_RMC,
        NMEA_MESSAGE_MAX,
};

enum EN_NMEA_LATITUDE_DIR {

	NMEA_LATITUDE_DIR_NORTH,
	NMEA_LATITUDE_DIR_SOUTH,
};

enum EN_NMEA_LONGITUDE_DIR {

	NMEA_LONGITUDE_DIR_EAST,
	NMEA_LONGITUDE_DIR_WEST,
};

enum EN_NMEA_GPS_QUALITY_FIX {

	NMEA_GPS_QUALITY_FIX_NO_FIX,
	NMEA_GPS_QUALITY_FIX_GPS_FIX,
	NMEA_GPS_QUALITY_FIX_OMNISTAR_FIX,
	NMEA_GPS_QUALITY_FIX_KIN_INTEGER_FIX,
	NMEA_GPS_QUALITY_FIX_KIN_FLOAT_FIX,
};

/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Data type definition
 *******************************************************************************
 */

typedef struct SNmeaUtcTime {
    uint16_t m_un16Year; /* year, to store the number in 4 decimal digits, such 2018 */
    uint8_t m_un8Month; /* month, [1, 12] */
    uint8_t m_un8MonthDay; /* day in a month, [1, 31] */
    uint8_t m_un8Hour; /* hour, [0, 23] */
    uint8_t m_un8Minute; /* minute, [0, 59] */
    uint8_t m_un8Second; /* second, [0, 60] */
    uint16_t m_un16Milliseconds; /* milliseconds, [0, 999] */
} SNmeaUtcTime;

typedef struct SNmeaGgaData {

        SNmeaUtcTime m_sUcTime;
        double m_dLatitude;
        enum EN_NMEA_LATITUDE_DIR m_eLatitudeDirection;
        double m_dLongitude;
        enum EN_NMEA_LONGITUDE_DIR m_eLongitudeDirection;
        enum EN_NMEA_GPS_QUALITY_FIX m_eGpsFix;
        int8_t m_n8NumberOfSatellites;

} SNmeaGgaData;

typedef struct SNmeaRmcData {

        SNmeaUtcTime m_sUcTime;
        double m_dLatitude;
        enum EN_NMEA_LATITUDE_DIR m_eLatitudeDirection;
        double m_dLongitude;
        enum EN_NMEA_LONGITUDE_DIR m_eLongitudeDirection;
        double m_dVelcoityInKnots;

} SNmeaRmcData;

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t nmea_get_gga_data(fix_data_t *psPotiFixData, SNmeaGgaData *psGgaData);
int32_t nmea_get_rmc_data(fix_data_t *psPotiFixData, SNmeaRmcData *psRmcData);
int32_t nmea_get_rmc_Data(SNmeaRmcData *psRmcData);

int32_t nmea_build_gga_msg(SNmeaGgaData *psGgaData, char *pchOutput);
int32_t nmea_build_gsa_msg(char *pchOutput);
int32_t nmea_build_rmc_msg(SNmeaRmcData *psRmcData, char *pchOutput);

double nmea_convert_decimal_degress_to_degrees_minutes(double dValue);
/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

static int8_t nmea_compute_sentence_checksum(char *pchSentence, int32_t n32SentenceSize);
static char nmea_convert_dec_to_hex(int8_t n8Number);

#endif
