#include "gps_sim.h"

#include <stdio.h>
#include "common/file/csv_infra.h"

/*
 *******************************************************************************
 * Private variables
 *******************************************************************************
 */

static bool m_bIsSimulatorEnabled = false;

static int32_t m_n32LatitudeArrayIndex = 0;
static int32_t m_n32LongitudeArrayIndex = 0;
static int32_t m_n32AltitudeArrayIndex = 0;

static double *m_pdLatitudeArray = NULL;
static double *m_pdLongitudeArray = NULL;
static double *m_pdAltitudeArray = NULL;

static bool m_bIsPausingFixData = false;

static int32_t m_n32LatitudeArraySize = 0;
static int32_t m_n32LongitudeArraySize = 0;
static int32_t m_n32AltitudeArraySize = 0;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t gps_sim_init(const char *pchScenarioName, const char *pchParticipantId) {

    // Set global variables.
    m_bIsPausingFixData = false;

    printf("Loading GPS Simulator - Using scenario %s, Participant ID: %s, Sync Station: %d\n", pchScenarioName, pchParticipantId, g_sLocalScenarioInfo.m_un32GpSimSyncId);

    // Init internal variables.
    m_bIsSimulatorEnabled = true;

    m_n32LatitudeArrayIndex = 0;
    m_n32LongitudeArrayIndex = 0;
    m_n32AltitudeArrayIndex = 0;

    m_pdLatitudeArray = NULL;
    m_pdLongitudeArray = NULL;
    m_pdAltitudeArray = NULL;

    m_n32LatitudeArraySize = 0;
    m_n32LongitudeArraySize = 0;
    m_n32AltitudeArraySize = 0;

    char achFilePath[1000];

    sprintf(achFilePath, "%s/simulator/%s/%s/latitude.csv", g_pchConfigurationFileDirectoryPath, pchScenarioName, pchParticipantId);
    m_n32LatitudeArraySize = read_csv_doubles(achFilePath, &m_pdLatitudeArray);

    sprintf(achFilePath, "%s/simulator/%s/%s/longitude.csv", g_pchConfigurationFileDirectoryPath, pchScenarioName, pchParticipantId);
    m_n32LongitudeArraySize = read_csv_doubles(achFilePath, &m_pdLongitudeArray);

    sprintf(achFilePath, "%s/simulator/%s/%s/altitude.csv", g_pchConfigurationFileDirectoryPath, pchScenarioName, pchParticipantId);
    m_n32AltitudeArraySize = read_csv_doubles(achFilePath, &m_pdAltitudeArray);

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    // Check loaded data.
    if(0 == m_n32LatitudeArraySize
            || 0 == m_n32LongitudeArraySize) {

        printf("Requested longitude and latitude csv files are not the same size\n");

        gps_sim_release();
        n32Result = PROCEDURE_INVALID_PARAMETERS_ERROR;

    } else if(m_n32LatitudeArraySize != m_n32LongitudeArraySize) {

        printf("Requested longitude and latitude csv files are not the same size\n");

        gps_sim_release();
        n32Result = PROCEDURE_INVALID_PARAMETERS_ERROR;

    } else if(m_n32LatitudeArraySize != m_n32AltitudeArraySize
              && 0 != m_n32AltitudeArraySize) {

        printf("Requested longitude, latitude and altitude csv files are not the same size\n");

        gps_sim_release();
        n32Result = PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    printf("Longitude array size: %d, Latitude array size: %d\n", m_n32LatitudeArraySize, m_n32LongitudeArraySize);

    return n32Result;
}

void gps_sim_update_fix_data(fix_data_t *psPotiFixData) {

    if(false == m_bIsSimulatorEnabled) {

        return;
    }

    //printf("Using simulated GPS fix data\n");

    psPotiFixData->latitude = m_pdLatitudeArray[m_n32LatitudeArrayIndex];
    psPotiFixData->longitude = m_pdLongitudeArray[m_n32LongitudeArrayIndex];

    if(true == m_bIsPausingFixData) {

        return;
    }

    //printf("Updating scenario indexes...\n");

    m_n32LatitudeArrayIndex = (m_n32LatitudeArrayIndex + 1) % m_n32LatitudeArraySize;
    m_n32LongitudeArrayIndex = (m_n32LongitudeArrayIndex + 1) % m_n32LongitudeArraySize;
}

void gps_sim_pause_fix_data(bool bIsPaused) {

    m_bIsPausingFixData = bIsPaused;

    if(true == m_bIsPausingFixData) {

        printf("GPS simulator is pausing\n");

    } else {

        printf("GPS simulator is not pausing\n");
    }
}

bool gps_sim_is_paused() {

    return m_bIsPausingFixData;
}

void gps_sim_release() {

    if(0 != m_n32LatitudeArraySize) {

        free(m_pdLatitudeArray);
        m_n32LatitudeArraySize = 0;
    }

    if(0 != m_n32LongitudeArraySize) {

        free(m_pdLongitudeArray);
        m_n32LongitudeArraySize = 0;
    }

    if(0 != m_n32AltitudeArraySize) {

        free(m_pdAltitudeArray);
        m_n32AltitudeArraySize = 0;
    }

    m_bIsSimulatorEnabled = false;
}