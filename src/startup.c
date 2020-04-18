
#include "threads.h"

#include "lib/inc/error_code_user.h"

#include "common/file/ini_infra.h"

#include "common/containers/array_queue.h"
#include "common/containers/spsc_array_queue.h"

#include "boundary/serial_boundary.h"
#include "boundary/ethernet_boundary.h"

#include "sa/sa_mngr.h"

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

/**
 * @brief ini_value_loader
 * Save value from ini file field.
 *
 * @param pchUser - The requested user info - Indicates the file we are currently loading.
 * @param pchSection - The section from ini file.
 * @param pchName - The field name under the above section from the ini file.
 * @param pchValue - The value from the above field under the abovex2 section from the ini file.
 * @return - 0 if successfull otherwise a negative value that indicates an error.
 */
int32_t ini_value_loader(void* pchUser, const char* pchSection, const char* pchName, const char* pchValue) {

    char *pchError = NULL;
    int32_t n32Reuslt = PROCEDURE_SUCCESSFULL;

    if(0 == strcmp(CONFIGURATION_FILE_STATION_INFO_USER, pchUser)) {

        if(0 == strcmp("station_info", pchSection)) {

            if(0 == strcmp("station_id", pchName)) {

                g_sStationInfo.m_un32StationId = strtol(pchValue, &pchError, 10);

            } else if(0 == strcmp("station_type", pchName)) {

                g_sStationInfo.m_n32StationType = strtol(pchValue, &pchError, 10);
            }
        }

    } else if(0 == strcmp(CONFIGURATION_FILE_GENERAL_PARAMS_USER, pchUser)) {

        if(0 == strcmp("tx_params", pchSection)) {

            if(0 == strcmp("tx_frequency_in_10_hz", pchName)) {

                g_sTxParameters.m_n32TxFrequencyIn10Hz = strtol(pchValue, &pchError, 10);
            }
        }

    } else if(0 == strcmp(CONFIGURATION_FILE_SCENARIO_PARAMS_USER, pchUser)) {

        if(0 == strcmp("scenario_info", pchSection)) {

            if(false == m_bIsScenarioLoaded) {

                if(0 == strcmp("scenario_en", pchName)) {

                    if(0 < strtol(pchValue, &pchError, 10)) {

                        g_sScenarioInfo.m_bIsScenarioEnabled = true;
                    }

                } else if(true == g_sScenarioInfo.m_bIsScenarioEnabled) {

                    if(0 == strcmp("scenario_name", pchName)) {

                        memcpy(g_sScenarioInfo.m_achName, pchValue, strlen(pchValue) + 1);

                    } else if(0 == strcmp("scenario_gps_sim_en", pchName)) {

                        g_sScenarioInfo.m_bIsGpsSimEnabled = strtol(pchValue, &pchError, 10);

                    } else if(0 == strcmp("scenario_gps_sim_id", pchName)) {

                        memcpy(g_sScenarioInfo.m_achParticipantId, pchValue, strlen(pchValue) + 1);

                    } else if(0 == strcmp("scenario_gps_sim_sync_id", pchName)) {

                        g_sScenarioInfo.m_un32GpSimSyncId = strtol(pchValue, &pchError, 10);
                        m_bIsScenarioLoaded = true;
                    }
                }
            }
        }
    }

    return n32Reuslt;
}

/**
 * @brief print_program_arguments
 */
void print_program_arguments() {

    printf("-------------------------------------\n");
    printf("Using configuration directory path - %s\n", g_pchConfigurationFileDirectoryPath);
    printf("-------------------------------------\n");
}

/**
 * @brief print_configuration_parameters
 */
void print_configuration_parameters() {

    printf("-------------------------------------\n");
    printf("Loaded station info - ID: %d, Type: %d\n", g_sStationInfo.m_un32StationId, g_sStationInfo.m_n32StationType);
    printf("-------------------------------------\n");
}

/**
 * @brief main
 *
 * @param argc
 * @param argv
 * @return
 */
int32_t main(int argc, char **argv) {

    // Check arguments.
    if(1 >= argc) {

        printf("Missing program arguments!\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    // Set global variables.
    g_pchConfigurationFileDirectoryPath = argv[1];

    print_program_arguments();

    // -------------------------------------------------
    // ------------- Set Signal Catching ---------------
    // -------------------------------------------------

    // Catch error signals.
    struct sigaction sErrorSigAction;

    memset(&sErrorSigAction, 0, sizeof(sErrorSigAction));
    sigemptyset(&sErrorSigAction.sa_mask);

    sErrorSigAction.sa_handler = error_catcher_active;
    sErrorSigAction.sa_flags = SA_SIGINFO;

    sigaction(SIGSEGV, &sErrorSigAction, NULL);
    sigaction(SIGABRT, &sErrorSigAction, NULL);
    sigaction(SIGFPE, &sErrorSigAction, NULL);

    // Catch exit signals.
    struct sigaction sExitSigAction;

    memset(&sExitSigAction, 0, sizeof(sExitSigAction));
    sigemptyset(&sExitSigAction.sa_mask);

    sExitSigAction.sa_handler = exit_catcher_active;
    sExitSigAction.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &sExitSigAction, NULL);

    // -------------------------------------------------
    // ----------- Load Configuration Files ------------
    // -------------------------------------------------

    // Reset configuration file fields.
    memset(&g_sStationInfo, 0, sizeof(g_sStationInfo));
    memset(&g_sScenarioInfo, 0, sizeof(g_sScenarioInfo));
    memset(&g_sTxParameters, 0, sizeof(g_sTxParameters));

    m_bIsScenarioLoaded = true;

    int32_t n32ProcedureResult = 0;

    char achFilePath[100];

    sprintf(achFilePath, "%s/station_info.ini", g_pchConfigurationFileDirectoryPath);
    n32ProcedureResult &= ini_parse(achFilePath, ini_value_loader, CONFIGURATION_FILE_STATION_INFO_USER);
    sprintf(achFilePath, "%s/scenario_parameters.ini", g_pchConfigurationFileDirectoryPath);
    n32ProcedureResult &= ini_parse(achFilePath, ini_value_loader, CONFIGURATION_FILE_SCENARIO_PARAMS_USER);
    sprintf(achFilePath, "%s/general_parameters.ini", g_pchConfigurationFileDirectoryPath);
    n32ProcedureResult &= ini_parse(achFilePath, ini_value_loader, CONFIGURATION_FILE_GENERAL_PARAMS_USER);

    if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

        printf("Cannot read configuration files\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    print_configuration_parameters();

    // -------------------------------------------------
    // -------------- Initialize Services --------------
    // -------------------------------------------------

#ifdef __SERIAL_OUTPUT_ENABLED__

    g_fpBoundaryWriter = serial_boundary_write;

#else

    g_fpBoundaryWriter = ethernet_boundary_write;

#endif

    // Initialize containers.
    n32ProcedureResult &= array_queue_init();
    n32ProcedureResult &= spsc_array_queue_init();

    if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

        printf("Cannot init containers\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    // Initialize SA manager.
    n32ProcedureResult &= sa_mngr_init();

    if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

        printf("Cannot init situation awarnesss manager\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    // -------------------------------------------------
    // -------------- Initialize Threads ---------------
    // -------------------------------------------------

    //  Activate tx tasks.
    pthread_create(&g_asThreads[0], NULL, sa_processor_active, NULL);
    pthread_create(&g_asThreads[1], NULL, poti_receiver_active, NULL);
    pthread_create(&g_asThreads[2], NULL, cam_receiver_active, NULL);
    pthread_create(&g_asThreads[3], NULL, denm_receiver_active, NULL);

    // If threads are done then so is the controller.
    pthread_join(g_asThreads[3], NULL);
    pthread_join(g_asThreads[2], NULL);
    pthread_join(g_asThreads[1], NULL);
    pthread_join(g_asThreads[0], NULL);

    // -------------------------------------------------
    // --------------- Release Resources ---------------
    // -------------------------------------------------

    printf("Releasing all resources...\n");

    // Shutdown situation awareness manager.
    sa_mngr_release();

    // Release containers.
    spsc_array_queue_release();
    array_queue_release();

    return PROCEDURE_SUCCESSFULL;
}
