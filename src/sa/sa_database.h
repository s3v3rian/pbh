#ifndef SA_SA_DATABASE_H_
#define SA_SA_DATABASE_H_

#include "common/globals.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

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

typedef struct SStationLLAData {

    double m_dLongitude;
    double m_dLatitude;
    double m_dAltitude;

} SStationLLAData;

typedef struct SStationFusionData {

    uint32_t m_un32StationId;
    SStationLLAData m_sCurrentLLA;

} SStationFusionData;

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

void sa_database_init();
SStationFusionData *sa_database_get_station_data(uint32_t un32StationId);

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


