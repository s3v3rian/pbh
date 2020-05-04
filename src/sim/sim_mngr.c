#include "sim_mngr.h"

#include "lib/inc/gn_public.h"

#include "common/file/ini_infra.h"
#include "common/sa_database.h"

#include "sim/gps_sim.h"

/*
 *******************************************************************************
 * Private function signatures
 *******************************************************************************
 */

int32_t sim_mngr_ini_loader(void* pchUser, const char* pchSection, const char* pchName, const char* pchValue);

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

bool m_bIsScenarioLoaded;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t sim_mngr_init() {

    char achFilePath[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    sprintf(achFilePath, "%s/scenario_parameters.ini", g_pchConfigurationFileDirectoryPath);
    ini_parse(achFilePath, sim_mngr_ini_loader, CONFIGURATION_FILE_SCENARIO_PARAMS_USER);
    sprintf(achFilePath, "%s/simulator/%s/%s/station_info.ini", g_pchConfigurationFileDirectoryPath, g_sLocalScenarioInfo.m_achScenarioName, g_sLocalScenarioInfo.m_achParticipantId);
    ini_parse(achFilePath, sim_mngr_ini_loader, CONFIGURATION_FILE_STATION_INFO_USER);

    // Initialize GPS simulator.
    if(true == g_sLocalScenarioInfo.m_bIsScenarioEnabled) {

        if(PROCEDURE_SUCCESSFULL != gps_sim_init(g_sLocalScenarioInfo.m_achScenarioName, g_sLocalScenarioInfo.m_achParticipantId)) {

            printf("Cannot initialize gps simulator\n");
            return PROCEDURE_INVALID_SERVICE_INIT_ERROR;

        } else {

            gps_sim_set_is_pause_fix_data(true);
        }
    }

    return PROCEDURE_SUCCESSFULL;
}

void sim_mngr_release() {

    gps_sim_release();
}

void sim_mngr_process_fusion() {

    g_fp_sim_processor_do_fusion();
}

void sim_mngr_process_fix_data(fix_data_t *psPotiFixData) {

    gps_sim_update_fix_data(psPotiFixData);

    if(true == gps_sim_get_is_pause_fix_data()) {

        if(0 != g_sLocalScenarioInfo.m_un32GpSimSyncId
                && g_sLocalScenarioInfo.m_un32GpSimSyncId != g_sLocalStationInfo.m_un32StationId) {

            SStationFullFusionData *psFusionData = sa_database_get_station_data(g_sLocalScenarioInfo.m_un32GpSimSyncId);

            if(NULL != psFusionData) {

                printf("Received data from sync station id %d, unpausing the gps simulator\n", g_sLocalScenarioInfo.m_un32GpSimSyncId);

                gps_sim_set_is_pause_fix_data(false);
            }

        } else {

            gps_sim_set_is_pause_fix_data(false);
        }

    }
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

int32_t sim_mngr_ini_loader(void* pchUser, const char* pchSection, const char* pchName, const char* pchValue) {

    char *pchError = NULL;
    int32_t n32Reuslt = PROCEDURE_SUCCESSFULL;

    if(0 == strcmp(CONFIGURATION_FILE_SCENARIO_PARAMS_USER, pchUser)) {

        if(0 == strcmp("scenario_info", pchSection)) {

            if(false == m_bIsScenarioLoaded) {

                if(0 == strcmp("scenario_en", pchName)) {

                    if(0 < strtol(pchValue, &pchError, 10)) {

                        g_sLocalScenarioInfo.m_bIsScenarioEnabled = true;
                    }

                } else if(true == g_sLocalScenarioInfo.m_bIsScenarioEnabled) {

                    if(0 == strcmp("scenario_name", pchName)) {

                        memcpy(g_sLocalScenarioInfo.m_achScenarioName, pchValue, strlen(pchValue) + 1);

                    } else if(0 == strcmp("scenario_participant_id", pchName)) {

                        memcpy(g_sLocalScenarioInfo.m_achParticipantId, pchValue, strlen(pchValue) + 1);
                        m_bIsScenarioLoaded = true;
                    }
                }
            }
        }

    } else if(0 == strcmp(CONFIGURATION_FILE_STATION_INFO_USER, pchUser)) {

        if(0 == strcmp("station_info", pchSection)) {

            if(0 == strcmp("station_id", pchName)) {

                g_sLocalStationInfo.m_un32StationId = strtol(pchValue, &pchError, 10);

            } else if(0 == strcmp("station_type", pchName)) {

                g_sLocalStationInfo.m_n32StationType = strtol(pchValue, &pchError, 10);

            } else if(0 == strcmp("sub_station_type", pchName)) {

                g_sLocalStationInfo.m_n32SubStationType = strtol(pchValue, &pchError, 10);
            }
        }
    }

    return n32Reuslt;
}
