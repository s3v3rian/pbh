#include "gps_sim.h"

#include <stdio.h>

#include "common/file/csv_infra.h"
#include "common/utils/geo_utils.h"

/*
 *******************************************************************************
 * Private variables
 *******************************************************************************
 */

static bool m_bIsSimulatorEnabled = false;

static int32_t m_n32LatitudeArrayIndex = 0;
static int32_t m_n32LongitudeArrayIndex = 0;
static int32_t m_n32AltitudeArrayIndex = 0;
static int32_t m_n32BearingArrayIndex = 0;

static double *m_pdLatitudeArray = NULL;
static double *m_pdLongitudeArray = NULL;
static double *m_pdAltitudeArray = NULL;
static double *m_pdBearingArray = NULL;

static bool m_bIsPausingFixData = false;

static int32_t m_n32LatitudeArraySize = 0;
static int32_t m_n32LongitudeArraySize = 0;
static int32_t m_n32AltitudeArraySize = 0;
static int32_t m_n32BearingArraySize = 0;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t gps_sim_init(const char *pchScenarioName, const char *pchParticipantId) {

    // Set global variables.
    m_bIsPausingFixData = false;

    // Init internal variables.
    m_bIsSimulatorEnabled = true;

    m_n32LatitudeArrayIndex = 0;
    m_n32LongitudeArrayIndex = 0;
    m_n32AltitudeArrayIndex = 0;
    m_n32BearingArrayIndex = 0;

    m_pdLatitudeArray = NULL;
    m_pdLongitudeArray = NULL;
    m_pdAltitudeArray = NULL;
    m_pdBearingArray = NULL;

    m_n32LatitudeArraySize = 0;
    m_n32LongitudeArraySize = 0;
    m_n32AltitudeArraySize = 0;
    m_n32BearingArraySize = 0;

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
            || 0 == m_n32LongitudeArraySize
            || 0 == m_n32AltitudeArraySize) {

        printf("Simulator LLA csv files are zero size\n");

        gps_sim_release();
        n32Result = PROCEDURE_INVALID_PARAMETERS_ERROR;

    } else if(m_n32LatitudeArraySize != m_n32LongitudeArraySize
              || m_n32LatitudeArraySize != m_n32AltitudeArraySize) {

        printf("Simulator LLA csv files are not the same size\n");

        gps_sim_release();
        n32Result = PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    printf("Longitude array size: %d, Latitude array size: %d, Altitude array size: %d\n", m_n32LatitudeArraySize, m_n32LongitudeArraySize, m_n32AltitudeArraySize);

    m_pdBearingArray = calloc(m_n32LatitudeArraySize, sizeof(double));
    m_n32BearingArraySize = m_n32LatitudeArraySize;

    // Calculate bearing of all points.
    for(int32_t n32Index = 0; n32Index < m_n32BearingArraySize; n32Index++) {

        if(n32Index + 1 >= m_n32BearingArraySize) {

            m_pdBearingArray[n32Index] = m_pdBearingArray[n32Index - 1];
            break;
        }

        m_pdBearingArray[n32Index] = geodesic_calculate_bearing(
                m_pdLatitudeArray[n32Index],
                m_pdLongitudeArray[n32Index],
                m_pdLatitudeArray[n32Index + 1],
                m_pdLongitudeArray[n32Index + 1]);
    }

    return n32Result;
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

    if(0 != m_n32BearingArraySize) {

        free(m_pdBearingArray);
        m_n32BearingArraySize = 0;
    }

    m_bIsSimulatorEnabled = false;
}

void gps_sim_update_fix_data(fix_data_t *psPotiFixData) {

    if(false == m_bIsSimulatorEnabled) {

        return;
    }

    //printf("Using simulated GPS fix data\n");

    psPotiFixData->latitude = m_pdLatitudeArray[m_n32LatitudeArrayIndex];
    psPotiFixData->longitude = m_pdLongitudeArray[m_n32LongitudeArrayIndex];
    psPotiFixData->altitude = m_pdAltitudeArray[m_n32AltitudeArrayIndex];
    psPotiFixData->course_over_ground = m_pdBearingArray[m_n32BearingArrayIndex];
    psPotiFixData->horizontal_speed = 0.0;
    psPotiFixData->vertical_speed = 0.0;

    psPotiFixData->status = FIX_STATUS_GNSS;
    psPotiFixData->mode = FIX_MODE_3D;

    psPotiFixData->err_time = 0.0;
    psPotiFixData->err_smajor_orientation = 0.0;
    psPotiFixData->err_smajor_axis = 0.0;
    psPotiFixData->err_sminor_axis = 0.0;
    psPotiFixData->err_vertical_speed = 0.0;
    psPotiFixData->err_horizontal_speed = 0.0;
    psPotiFixData->err_course_over_ground = 0.0;
    psPotiFixData->err_altitude = 0.0;

    if(true == m_bIsPausingFixData) {

        return;
    }

    //printf("Updating scenario indexes...\n");

    m_n32LatitudeArrayIndex = (m_n32LatitudeArrayIndex + 1) % m_n32LatitudeArraySize;
    m_n32LongitudeArrayIndex = (m_n32LongitudeArrayIndex + 1) % m_n32LongitudeArraySize;
    m_n32AltitudeArrayIndex = (m_n32AltitudeArrayIndex + 1) % m_n32AltitudeArraySize;
    m_n32BearingArrayIndex = (m_n32BearingArrayIndex + 1) % m_n32BearingArraySize;
}

void gps_sim_set_is_pause_fix_data(bool bIsPaused) {

    m_bIsPausingFixData = bIsPaused;

    if(true == m_bIsPausingFixData) {

        printf("GPS simulator is pausing\n");

    } else {

        printf("GPS simulator is not pausing\n");
    }
}

bool gps_sim_get_is_pause_fix_data() {

    return m_bIsPausingFixData;
}

bool gps_sim_get_is_scenario_fix_data_starting_now() {

    return (0 == m_n32LatitudeArrayIndex);
}
