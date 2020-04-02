
#include <pthread.h>

#include "globals.h"
#include "gps_sim.h"
#include "time.h"

#include "cam_mngr.h"
#include "denm_mngr.h"

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

// All threads.
static pthread_t g_asThreads[8];

// Sender threads BTP handlers.
static btp_handler_ptr g_pBtpDenmHandler;

// POTI service handler.
static poti_service_t *g_psPotiHandler = NULL;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

static void *main_sender_func(void *p_param __attribute__((unused)));
static void *cam_receiver_func(void *p_param __attribute__((unused)));
static void *denm_receiver_func(void *p_param __attribute__((unused)));

/*
 *******************************************************************************
 *******************************************************************************
 */

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
int32_t main(int argc, char **argv) {

    // -------------------------------------------------
    // ----------- Initialize GPS Simulator ------------
    // -------------------------------------------------

    int32_t n32Result = 0;

#ifdef __GPS_SIMULATOR_ENABLED__

    if(2 <= argc) {

        n32Result = gps_sim_init(argv[1], argv[2]);

        if(0 > n32Result) {

            printf("Cannot initialize gps simulator\n");
            return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
        }
    }

#endif

    // -------------------------------------------------
    // ------------ Initialize POTI Service ------------
    // -------------------------------------------------

    n32Result = poti_create_service(&g_psPotiHandler, NULL);

    if(0 > IS_SUCCESS(n32Result)) {

        printf("Cannot create POTI service: %s\n", ERROR_MSG(n32Result));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    // -------------------------------------------------
    // ----------- Initialize Tx/Rx Threads ------------
    // -------------------------------------------------

    //  Activate tx tasks.
    pthread_create(&g_asThreads[0], NULL, main_sender_func, NULL);

    // Activate rx tasks.
    pthread_create(&g_asThreads[1], NULL, cam_receiver_func, NULL);
    pthread_create(&g_asThreads[2], NULL, denm_receiver_func, NULL);

    /* Wait till cam_sender_func is completed. */
    pthread_join(g_asThreads[0], NULL);

    // -------------------------------------------------
    // --------------- Release Resources ---------------
    // -------------------------------------------------

    /* Release POTI handler. */
    poti_release_service(g_psPotiHandler);

    return PROCEDURE_SUCCESSFULL;
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static void *main_sender_func(void *p_param __attribute__((unused))) {

    int32_t n32IsSenderActive = BOOLEAN_TRUE;

    fix_data_t sPotiFixData = FIX_DATA_INIT;
    uint32_t n32SendCount = 0;

    // -------------------------------------------------
    // ----------- Initialize Managers -------------
    // -------------------------------------------------

    cam_mngr_init();
    denm_mngr_init();

    int32_t n32Result = 0;

    while(BOOLEAN_TRUE == n32IsSenderActive) {

        /* Wait for new GNSS update (10 Hz by default). */
        n32Result = poti_wait_gnss_data(g_psPotiHandler, NULL);

        n32SendCount++;

        // Send every 1 second.
        if(SENDER_FREQUENCY_IN_10_HZ > n32SendCount) {

            continue;
        }

        n32SendCount = 0;

        if(0 > IS_SUCCESS(n32Result)) {

            printf("poti_wait_gnss_data error: %s\n", ERROR_MSG(n32Result));
            continue;
        }

        /* Get fix data. */
        n32Result = poti_get_fix_data(g_psPotiHandler, &sPotiFixData);

        if(0 > IS_SUCCESS(n32Result)) {

            printf("Cannot get GNSS fix data: %s\n", ERROR_MSG(n32Result));
            continue;
        }

        /* Make sure the navigation information is valid. */
        if(sPotiFixData.status == FIX_STATUS_NA
                || sPotiFixData.mode == FIX_MODE_NA
                || sPotiFixData.mode == FIX_MODE_NO_FIX) {

            printf("Navigation information is invalid\n");

        } else {

            // Use simulated values if requested.
            gps_sim_update_fix_data(&sPotiFixData);

            // Process poti info into cam manager.
            cam_mngr_process_tx(&sPotiFixData);
            denm_mngr_process_tx(&sPotiFixData);

            fflush(stdout);
        }
    }

    // -------------------------------------------------
    // --------------- Release Resources ---------------
    // -------------------------------------------------

    cam_mngr_release();
    denm_mngr_release();
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static void *denm_receiver_func(void *p_param __attribute__((unused))) {

    int32_t n32IsReceiverActive = BOOLEAN_TRUE;

    printf("Starting DENM receiver\n");

    while(BOOLEAN_TRUE == n32IsReceiverActive) {

        denm_mngr_process_rx();
    }

    printf("Stopping DENM receiver\n");

    /* Terminate this thread. */
    pthread_exit(NULL);
}

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
static void *cam_receiver_func(void *p_param __attribute__((unused))) {

    //pBtpCamHandler_ptr pBtpCamHandler; // TODO Why?
    int32_t n32IsReceiverActive = BOOLEAN_TRUE;

    printf("Starting CAM receiver\n");

    while(BOOLEAN_TRUE == n32IsReceiverActive) {

        cam_mngr_process_rx();
    }

    printf("Stopping CAM receiver\n");

    /* Terminate this thread. */
    pthread_exit(NULL);
}

