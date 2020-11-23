#include "sim_processor_bus.h"

#include "common/file/ini_infra.h"

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

int32_t sim_processor_bus_init() {

    printf("Initializing bus simulator processor\n");

    char achFilePath[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    sprintf(achFilePath, "%s/simulator/%s/%s/participant_parameters.ini", g_pchConfigurationFileDirectoryPath, g_sLocalScenarioInfo.m_achScenarioName, g_sLocalScenarioInfo.m_achParticipantId);
    ini_parse(achFilePath, sim_processor_ini_loader, CONFIGURATION_FILE_SCENARIO_PARTICIPANT_PARAMS_USER);

    return PROCEDURE_SUCCESSFULL;
}

void sim_processor_bus_do_fusion() {

    // No implementation.
}

void sim_processor_bus_gui() {

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
        }

    } else if(0 == strcmp("commercial_vehicle_info", pchSection)) {

        if(0 == strcmp("commercial_vehicle_dangerous_goods_present", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_bIsDangerousGoods = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("commercial_vehicle_dangerous_goods_type", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_eDangerousGoodsType = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("commercial_vehicle_trailer_present", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_bIsTrailerAttached = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("commercial_vehicle_trailer_height_in_meters", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_n32TrailerHeight = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("commercial_vehicle_trailer_length_in_meters", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_n32TrailerLength = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("commercial_vehicle_trailer_width_in_meters", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_n32TrailerWidth = strtol(pchValue, NULL, 10);

        } else if(0 == strcmp("commercial_vehicle_departure_location_latitude", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDepartureLocation.m_dLatitude = strtod(pchValue, NULL);

        } else if(0 == strcmp("commercial_vehicle_departure_location_longitude", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDepartureLocation.m_dLongitude = strtod(pchValue, NULL);

        } else if(0 == strcmp("commercial_vehicle_departure_location_altitude", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDepartureLocation.m_dAltitude = strtod(pchValue, NULL);

        } else if(0 == strcmp("commercial_vehicle_destination_location_latitude", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDestinationLocation.m_dLatitude = strtod(pchValue, NULL);

        } else if(0 == strcmp("commercial_vehicle_destination_location_longitude", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDestinationLocation.m_dLongitude = strtod(pchValue, NULL);

        } else if(0 == strcmp("commercial_vehicle_destination_location_altitude", pchName)) {

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDestinationLocation.m_dLatitude = strtod(pchValue, NULL);
        }
    }

    return PROCEDURE_SUCCESSFULL;
}
