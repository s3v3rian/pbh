#include "threads.h"

#include <time.h>

#include "common/globals.h"

/**
 * @brief main_sender_active
 * @param p_param
 * @return
 */
void *main_sender_active(void *p_param __attribute__((unused))) {

    int32_t n32IsSenderActive = BOOLEAN_TRUE;

    fix_data_t sPotiFixData = FIX_DATA_INIT;
    int32_t n32SendCount = 0;

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
        if(g_sTxParameters.m_n32TxFrequencyIn10Hz > n32SendCount) {

            continue;
        }

        n32SendCount = 0;

        if(false == IS_SUCCESS(n32Result)) {

            printf("poti_wait_gnss_data error: %s\n", ERROR_MSG(n32Result));
            continue;
        }

        /* Get fix data. */
        n32Result = poti_get_fix_data(g_psPotiHandler, &sPotiFixData);

        if(false == IS_SUCCESS(n32Result)) {

            printf("Cannot get GNSS fix data: %s\n", ERROR_MSG(n32Result));
            continue;
        }

        /* Make sure the navigation information is valid. */
        if(sPotiFixData.status == FIX_STATUS_NA
                || sPotiFixData.mode == FIX_MODE_NA
                || sPotiFixData.mode == FIX_MODE_NO_FIX) {

            printf("Navigation information is invalid\n");

        } else {

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

    /* Terminate this thread. */
    pthread_exit(NULL);
}

/**
 * activetion_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
void *denm_receiver_active(void *p_param __attribute__((unused))) {

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
 * activetion_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */
void *cam_receiver_active(void *p_param __attribute__((unused))) {

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

