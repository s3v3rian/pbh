#include "sim_processor_traffic_light.h"

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

bool m_bIsLightChangeDone;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t sim_processor_traffic_light_init() {

    printf("Initializing RSU simulator processor\n");

    char achFilePath[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    sprintf(achFilePath, "%s/simulator/%s/%s/participant_parameters.ini", g_pchConfigurationFileDirectoryPath, g_sLocalScenarioInfo.m_achScenarioName, g_sLocalScenarioInfo.m_achParticipantId);
    ini_parse(achFilePath, sim_processor_ini_loader, CONFIGURATION_FILE_SCENARIO_PARTICIPANT_PARAMS_USER);

    m_bIsLightChangeDone = false;

    return PROCEDURE_SUCCESSFULL;
}

void sim_processor_traffic_light_do_fusion() {

    if(true == gps_sim_get_is_pause_fix_data()) {

        return;
    }

    if(true == gps_sim_get_is_scenario_fix_data_starting_now()) {

        if(false == m_bIsLightChangeDone) {

            m_bIsLightChangeDone = true;

            g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight = !g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight;

            printf("Switching state of traffic light - Red light: %d\n", g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight);
        }

    } else {

        m_bIsLightChangeDone = false;
    }
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

    } else if(0 == strcmp("rsu_info", pchSection)) {

        if(0 == strcmp("rsu_min_signal_violation_bearing", pchName)) {

            g_sLocalStationInfo.m_sRsuInfo.m_dMinSignalViolationBearing = strtod(pchValue, NULL);

        } else if(0 == strcmp("rsu_max_signal_violation_bearing", pchName)) {

            g_sLocalStationInfo.m_sRsuInfo.m_dMaxSignalViolationBearing = strtod(pchValue, NULL);

        } else if(0 == strcmp("rsu_min_dangerous_situation_bearing", pchName)) {

            g_sLocalStationInfo.m_sRsuInfo.m_dMinDangerousSituationBearing = strtod(pchValue, NULL);

        } else if(0 == strcmp("rsu_max_dangerous_situation_bearing", pchName)) {

            g_sLocalStationInfo.m_sRsuInfo.m_dMaxDangerousSituationBearing = strtod(pchValue, NULL);
        }

    } else if(0 == strcmp("traffic_light_rsu_info", pchSection)) {

        if(0 == strcmp("traffic_light_rsu_signal_violation_distance_in_meters", pchName)) {

            g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_n32SignalViolationThresholdInMeters = strtol(pchValue, NULL, 10);
        }
    }

    return PROCEDURE_SUCCESSFULL;
}
