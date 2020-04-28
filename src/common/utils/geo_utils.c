#include "geo_utils.h"

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
