#include "sim_processor_passenger.h"

#include "common/file/ini_infra.h"

#include "sim/gps_sim.h"

/*
 *******************************************************************************
 * Private function signatures
 *******************************************************************************
 */

static int32_t sim_processor_ini_loader(void* pchUser, const char* pchSection, const char* pchName, const char* pchValue);

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

int32_t sim_processor_passenger_init() {

    printf("Initializing passenger simulator processor\n");

    char achFilePath[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    sprintf(achFilePath, "%s/simulator/%s/%s/participant_parameters.ini", g_pchConfigurationFileDirectoryPath, g_sLocalScenarioInfo.m_achScenarioName, g_sLocalScenarioInfo.m_achParticipantId);
    ini_parse(achFilePath, sim_processor_ini_loader, CONFIGURATION_FILE_SCENARIO_PARTICIPANT_PARAMS_USER);

    return PROCEDURE_SUCCESSFULL;
}

void sim_processor_passenger_do_fusion() {

    // No implementation.
}

void sim_processor_passenger_gui() {

    // No implementation.
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

int32_t sim_processor_ini_loader(void* pchUser, const char* pchSection, const char* pchName, const char* pchValue) {

    if(0 == strcmp("participant_info", pchSection)) {

        if(0 == strcmp("participant_sync_id", pchName)) {

            g_sLocalScenarioInfo.m_un32GpSimSyncId = strtol(pchValue, NULL, 10);
        }

    } else if(0 == strcmp("vehicle_info", pchSection)) {

        if(0 == strcmp("vehicle_driver_name", pchName)) {

            memcpy(g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleDriverName, pchValue, strlen(pchValue) + 1);
            g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleDriverName[strlen(pchValue) + 1] = '\0';

        } else if(0 == strcmp("vehicle_license_plate", pchName)) {

            memcpy(g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleLicensePlate, pchValue, strlen(pchValue) + 1);
            g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleLicensePlate[strlen(pchValue) + 1] = '\0';

        } else if(0 == strcmp("vehicle_weight_in_kg", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_n32VehicleWeight = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("vehicle_height_in_meters", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_n32VehicleHeight = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("vehicle_length_in_meters", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_n32VehicleLength = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("vehicle_width_in_meters", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_n32VehicleWidth = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("vehicle_speed_in_kph", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_dVehicleSpeedInKph = strtod(pchValue, NULL);

        } else if(0 == strcmp("vehicle_collision_warning_threshold_in_meters", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_dCollisionWarningThresholdInMeters = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("vehicle_min_signal_violation_bearing", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_dMinSignalViolationBearing = strtod(pchValue, NULL);

        } else if(0 == strcmp("vehicle_max_signal_violation_bearing", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_dMaxSignalViolationBearing = strtod(pchValue, NULL);

        } else if(0 == strcmp("vehicle_min_dangerous_situation_bearing", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_dMinDangerousSituationBearing = strtod(pchValue, NULL);

        } else if(0 == strcmp("vehicle_max_dangerous_situation_bearing", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_dMaxDangerousSituationBearing = strtod(pchValue, NULL);
        }
    }
}
