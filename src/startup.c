
#include "threads.h"

#include "lib/inc/error_code_user.h"

#include "common/file/ini_infra.h"

#include "common/containers/array_queue.h"
#include "common/containers/spsc_array_queue.h"
#include "common/containers/blocked_array_queue.h"
#include "common/containers/ring_buffer.h"

#include "boundary/serial_boundary.h"
#include "boundary/ethernet_boundary.h"

#include "sa/processors/its_msg_processor_commercial.h"
#include "sa/processors/its_msg_processor_rsu.h"
#include "sa/sa_mngr.h"

#include "sim/processors/sim_processor_commercial.h"
#include "sim/processors/sim_processor_rsu.h"

#include "sim/sim_mngr.h"

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
int32_t station_info_ini_loader(void* pchUser, const char* pchSection, const char* pchName, const char* pchValue) {

    char *pchError = NULL;
    int32_t n32Reuslt = PROCEDURE_SUCCESSFULL;

    if(0 == strcmp("station_info", pchSection)) {

        if(0 == strcmp("station_id", pchName)) {

            g_sLocalStationInfo.m_un32StationId = strtol(pchValue, &pchError, 10);

        } else if(0 == strcmp("station_type", pchName)) {

            g_sLocalStationInfo.m_n32StationType = strtol(pchValue, &pchError, 10);

        } else if(0 == strcmp("sub_station_type", pchName)) {

            g_sLocalStationInfo.m_n32SubStationType = strtol(pchValue, &pchError, 10);
        }
    }

    return n32Reuslt;
}

int32_t general_parameters_ini_loader(void* pchUser, const char* pchSection, const char* pchName, const char* pchValue) {

    char *pchError = NULL;
    int32_t n32Reuslt = PROCEDURE_SUCCESSFULL;

    if(0 == strcmp("tx_params", pchSection)) {

        if(0 == strcmp("tx_frequency_in_10_hz", pchName)) {

            g_sLocalStationInfo.m_sParameters.m_n32TxFrequencyIn10Hz = strtol(pchValue, &pchError, 10);
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
    printf("Loaded station info - ID: %d, Type: %d\n", g_sLocalStationInfo.m_un32StationId, g_sLocalStationInfo.m_n32StationType);
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
    memset(&g_sLocalStationInfo, 0, sizeof(g_sLocalStationInfo));
    memset(&g_sLocalScenarioInfo, 0, sizeof(g_sLocalScenarioInfo));

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    char achFilePath[100];

    sprintf(achFilePath, "%s/station_info.ini", g_pchConfigurationFileDirectoryPath);
    ini_parse(achFilePath, station_info_ini_loader, CONFIGURATION_FILE_STATION_INFO_USER);
    sprintf(achFilePath, "%s/general_parameters.ini", g_pchConfigurationFileDirectoryPath);
    ini_parse(achFilePath, general_parameters_ini_loader, CONFIGURATION_FILE_GENERAL_PARAMS_USER);

    if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

        printf("Cannot read configuration files\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    printf("Successfully loaded configuration files\n");

#ifdef __EN_SIMULATOR_FEATURE__

    printf("Initializing simulator\n");

    // Set station type callbacks.
    switch(g_sLocalStationInfo.m_n32StationType) {

        case GN_ITS_STATION_HEAVY_TRUCK:

            g_fp_sim_processor_init = sim_processor_commercial_init;
            g_fp_sim_processor_do = sim_processor_commercial_do;
            break;

        case GN_ITS_STATION_ROAD_SIDE_UNIT:

            g_fp_sim_processor_init = sim_processor_rsu_init;
            g_fp_sim_processor_do = sim_processor_rsu_do;
            break;

        default:
            break;
    }

    n32ProcedureResult |= sim_mngr_init();

    if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

        printf("Cannot initialize simulator\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

#endif

    print_configuration_parameters();

    // -------------------------------------------------
    // -------------- Initialize Services --------------
    // -------------------------------------------------

#ifdef __EN_SERIAL_OUTPUT_FEATURE__

    printf("Set boundary writer to serial interface\n");
    g_fp_write_to_boundary = serial_boundary_write;

#else

    printf("Set boundary writer to ethernet interface\n");
    g_fpBoundaryWriter = ethernet_boundary_write;

#endif

    printf("Initializing containers\n");

    // Initialize containers.
    n32ProcedureResult |= array_queue_init();
    n32ProcedureResult |= spsc_array_queue_init();
    n32ProcedureResult |= blocked_array_queue_init();
    n32ProcedureResult |= ring_buffer_init();

    if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

        printf("Cannot init containers\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    printf("Successfully initialized containers\n");

    // -------------------------------------------------
    // --------- Initialize Situation Awarness ---------
    // -------------------------------------------------

    printf("Initializing SA manager...\n");

    switch(g_sLocalStationInfo.m_n32StationType) {

        case GN_ITS_STATION_HEAVY_TRUCK:

            g_fp_its_processor_init = its_msg_processor_cm_init;
            g_fp_its_processor_process_poti = its_msg_processor_cm_process_poti;
            g_fp_its_processor_proccess_cam = its_msg_processor_cm_process_rx_cam;
            g_fp_its_processor_proccess_denm = its_msg_processor_cm_process_rx_denm;
            break;

        case GN_ITS_STATION_ROAD_SIDE_UNIT:

            g_fp_its_processor_init = its_msg_processor_rsu_init;
            g_fp_its_processor_process_poti = its_msg_processor_rsu_process_poti;
            g_fp_its_processor_proccess_cam = its_msg_processor_rsu_process_rx_cam;
            g_fp_its_processor_proccess_denm = its_msg_processor_rsu_process_rx_denm;
            break;

        default:

            printf("Cannot set sa callbacks, unsupported its station type\n");
            return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    // Initialize SA manager.
    n32ProcedureResult |= sa_mngr_init();

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

#ifdef __EN_SIMULATOR_FEATURE__

    sim_mngr_release();

#endif

    // Shutdown situation awareness manager.
    sa_mngr_release();

    // Release containers.
    spsc_array_queue_release();
    array_queue_release();

    return PROCEDURE_SUCCESSFULL;
}
