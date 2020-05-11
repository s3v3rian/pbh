#include "geo_utils.h"

#include "math.h"

#include "common/globals.h"

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

double geodesic_convert_decimal_degress_to_degrees_minutes(double dValue) {

    double dFrac = dValue - (uint64_t)dValue;
    dFrac = dFrac * 60.0;
    double dNewValue = (uint64_t)dValue;
    dNewValue = dNewValue * 100.0;
    dNewValue = dNewValue + dFrac;

    return dNewValue;
}

double geodesic_convert_decimal_degrees_to_1_10_th_micro_degree(double dValue) {

    return dValue * 10000000.0;
}

double geodesic_calculate_bearing(double dLatitudeA, double dLongitudeA, double dLatitudeB, double dLongitudeB) {

    double pi = 3.141592653589793;
    double dValueX = cos(dLatitudeB) * sin(dLongitudeB - dLongitudeA);
    double dValueY = cos(dLatitudeA) * sin(dLatitudeB) - sin(dLatitudeA) * cos(dLatitudeB) * cos(dLongitudeB - dLongitudeA);
    double dBearing = atan2(dValueX, dValueY);
    dBearing = (dBearing * (180.0/pi) + 360.0);

    if(dBearing >= 360.0) {

        dBearing = dBearing - 360.0;
    }

    return dBearing;
}
