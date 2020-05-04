#include "alg_haversine.h"

#include <math.h>

/*
 *******************************************************************************
 * Private constant value definition
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private macros
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private/Extern data type definition
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private/Extern variables
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

double alg_haversine_compute(SDistanceData *psLocalLLAData, SDistanceData *psRemoteLLAData) {

    double pi = 3.141592653589793;
    int R = 6371e3; //Radius of the Earth
    double dLocalLat = psLocalLLAData->m_dLatitude;
    double dLocalLon = psLocalLLAData->m_dLongitude;
    double dRemoteLat = psRemoteLLAData->m_dLatitude;
    double dRemoteLon = psRemoteLLAData->m_dLongitude;

    //printf("Local - Lat: %f, Lon: %f, Remote - Lat: %f, Lon: %f\n", dLocalLat, dLocalLon, dRemoteLat, dRemoteLon);

    //dLocalLat = (pi/180)*(dLocalLat);
    //dRemoteLat = (pi/180)*(dRemoteLat);
    double differenceLon = (pi/180)*(dRemoteLon - dLocalLon);
    double differenceLat = (pi/180)*(dRemoteLat - dLocalLat);
    double a = sin(differenceLat/2) * sin(differenceLat/2) +
    cos(dLocalLat) * cos(dRemoteLat) * sin(differenceLon/2) * sin(differenceLon/2);
    double c = 2 * asin(sqrt(a));
    double distance = R * c;
    return distance;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
