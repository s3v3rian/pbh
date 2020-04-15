#include "gps_sim.h"

#include <stdio.h>
#include "common/file/csv_infra.h"

/*
 *******************************************************************************
 * Private variables
 *******************************************************************************
 */

static int32_t m_n32IsSimulatorEnabled = 0;

static int32_t m_n32LatitudeArrayIndex;
static int32_t m_n32LongitudeArrayIndex;
static int32_t m_n32AltitudeArrayIndex;

static double *m_pdLatitudeArray;
static double *m_pdLongitudeArray;
static double *m_pdAltitudeArray;

static int32_t m_n32IsPausingFixData;

static int32_t m_n32LatitudeArraySize;
static int32_t m_n32LongitudeArraySize;
static int32_t m_n32AltitudeArraySize;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t gps_sim_init(const char *pchScenarioName, const char *pchParticipantId) {

    // Set global variables.
    m_n32IsPausingFixData = BOOLEAN_FALSE;

    printf("Loading GPS Simulator - Using scenario %s, Participant ID: %s\n", pchScenarioName, pchParticipantId);

    // Init internal variables.
    m_n32IsSimulatorEnabled = 1;

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

    sprintf(achFilePath, "%s/gps_sim/%s/%s/latitude.csv", g_pchConfigurationFileDirectoryPath, pchScenarioName, pchParticipantId);
    m_n32LatitudeArraySize = read_csv_doubles(achFilePath, &m_pdLatitudeArray);

    sprintf(achFilePath, "%s/gps_sim/%s/%s/longitude.csv", g_pchConfigurationFileDirectoryPath, pchScenarioName, pchParticipantId);
    m_n32LongitudeArraySize = read_csv_doubles(achFilePath, &m_pdLongitudeArray);

    sprintf(achFilePath, "%s/gps_sim/%s/%s/altitude.csv", g_pchConfigurationFileDirectoryPath, pchScenarioName, pchParticipantId);
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

    return n32Result;
}

void gps_sim_update_fix_data(fix_data_t *psPotiFixData) {

    if(0 == m_n32IsSimulatorEnabled) {

        return;
    }

    //printf("Using simulated GPS fix data\n");

    if(0 != m_n32LatitudeArraySize) {

        if(BOOLEAN_FALSE == m_n32IsPausingFixData) {

            m_n32LatitudeArrayIndex = (m_n32LatitudeArrayIndex + 1) % m_n32LatitudeArraySize;
        }
        psPotiFixData->latitude = m_pdLatitudeArray[m_n32LatitudeArrayIndex];
    }

    if(0 != m_n32LongitudeArraySize) {

        if(BOOLEAN_FALSE == m_n32IsPausingFixData) {

            m_n32LongitudeArrayIndex = (m_n32LongitudeArrayIndex + 1) % m_n32LongitudeArraySize;
        }
        psPotiFixData->longitude = m_pdLongitudeArray[m_n32LongitudeArrayIndex];
    }

    if(0 != m_n32AltitudeArraySize) {

        if(BOOLEAN_FALSE == m_n32IsPausingFixData) {

            m_n32AltitudeArrayIndex = (m_n32AltitudeArrayIndex + 1) % m_n32AltitudeArraySize;
        }
        psPotiFixData->altitude = m_pdAltitudeArray[m_n32AltitudeArrayIndex];
    }
}

void gps_sim_pause_fix_data(int32_t n32IsPaused) {

    m_n32IsPausingFixData = n32IsPaused;

    if(BOOLEAN_TRUE == m_n32IsPausingFixData) {

        printf("GPS simulator is pausing");

    } else {

        printf("GPS simulator is not pausing");
    }
}

int32_t gps_sim_is_paused() {

    return m_n32IsPausingFixData;
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
}
