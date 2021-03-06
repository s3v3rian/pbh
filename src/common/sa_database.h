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
SStationFullFusionData *sa_database_add_station_data(uint32_t un32StationId);
SStationFullFusionData *sa_database_get_station_data(uint32_t un32StationId);

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


