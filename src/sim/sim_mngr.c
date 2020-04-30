#include "sim_mngr.h"

#include "lib/inc/gn_public.h"

#include "common/file/ini_infra.h"

#include "sa/sa_database.h"

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

    // Initialize GPS simulator.
    if(true == g_sLocalScenarioInfo.m_bIsScenarioEnabled) {

        if(PROCEDURE_SUCCESSFULL != gps_sim_init(g_sLocalScenarioInfo.m_achName, g_sLocalScenarioInfo.m_achParticipantId)) {

            printf("Cannot initialize gps simulator\n");
            return PROCEDURE_INVALID_SERVICE_INIT_ERROR;

        } else {

            gps_sim_pause_fix_data(true);

            if(0 == g_sLocalScenarioInfo.m_un32GpSimSyncId
                || g_sLocalScenarioInfo.m_un32GpSimSyncId == g_sLocalStationInfo.m_un32StationId) {

                gps_sim_pause_fix_data(false);
            }
        }
    }

    return g_fp_sim_processor_init();
}

void sim_mngr_gps_sim_update_fix_data(fix_data_t *psPotiFixData) {

    gps_sim_update_fix_data(psPotiFixData);

    if(true == g_sLocalScenarioInfo.m_bIsGpsSimEnabled
            && 0 != g_sLocalScenarioInfo.m_un32GpSimSyncId) {

        if(true == gps_sim_is_paused()) {

            SITSStationFusionData *psStationData = sa_database_get_station_data(g_sLocalScenarioInfo.m_un32GpSimSyncId);

            if(NULL != psStationData) {

                gps_sim_pause_fix_data(false);
            }
        }
    }
}

void sim_mngr_gps_sim_pause_fix_data(bool bIsPaused) {

    gps_sim_pause_fix_data(bIsPaused);
}

bool sim_mngr_gps_sim_is_paused() {

    return gps_sim_is_paused();
}

void sim_mngr_release() {

    gps_sim_release();
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

                        memcpy(g_sLocalScenarioInfo.m_achName, pchValue, strlen(pchValue) + 1);

                    } else if(0 == strcmp("scenario_gps_sim_en", pchName)) {

                        g_sLocalScenarioInfo.m_bIsGpsSimEnabled = strtol(pchValue, &pchError, 10);

                    } else if(0 == strcmp("scenario_gps_sim_id", pchName)) {

                        memcpy(g_sLocalScenarioInfo.m_achParticipantId, pchValue, strlen(pchValue) + 1);

                    } else if(0 == strcmp("scenario_gps_sim_sync_id", pchName)) {

                        g_sLocalScenarioInfo.m_un32GpSimSyncId = strtol(pchValue, &pchError, 10);
                        m_bIsScenarioLoaded = true;
                    }
                }
            }
        }
    }

    return n32Reuslt;
}
