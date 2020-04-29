#include "sa_database.h"

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

SITSStationFusionData m_asStationDb[200]; // TODO Change this to hash map.

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

void sa_database_init() {

    for(uint32_t un32Index = 0; un32Index < 200; un32Index++) {

        m_asStationDb[un32Index].m_un32StationId = 0;
    }
}

SITSStationFusionData *sa_database_add_station_data(uint32_t un32StationId) {

    if(0 == un32StationId) {

        printf("Cannot station with 0 station id\n");
        return NULL;
    }

    SITSStationFusionData *psStationData = NULL;

    for(uint32_t un32Index = 0; un32Index < 200; un32Index++) {

        if(0 == m_asStationDb[un32Index].m_un32StationId) {

            psStationData = &m_asStationDb[un32Index];
            psStationData->m_un32StationId = un32StationId;
            break;
        }
    }

    return psStationData;
}

SITSStationFusionData *sa_database_get_station_data(uint32_t un32StationId) {

    SITSStationFusionData *psStationData = NULL;

    for(uint32_t un32Index = 0; un32Index < 200; un32Index++) {

        if(un32StationId == m_asStationDb[un32Index].m_un32StationId) {

            psStationData = &m_asStationDb[un32Index];
            break;
        }
    }

    return psStationData;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
