#include "threads.h"

#include <time.h>

#include "common/globals.h"
#include "sa/sa_mngr.h"

/*
 *******************************************************************************
 * Private/Extern variables
 *******************************************************************************
 */

int32_t m_bIsThreadsActive = true;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

/**
 * @brief main_sender_active
 * @param p_param
 * @return
 */
void *main_sender_active(void *p_param __attribute__((unused))) {

    printf("Starting sender\n");

    // -------------------------------------------------
    // ----------- Initialize Managers -------------
    // -------------------------------------------------

    sa_mngr_init();

    // -------------------------------------------------
    // --------------- Start POTI Loop -----------------
    // -------------------------------------------------

    while(true == m_bIsThreadsActive) {

        sa_mngr_process_tx();
    }

    printf("Stopping sender\n");

    // -------------------------------------------------
    // --------------- Release Resources ---------------
    // -------------------------------------------------

    sa_mngr_release();

    /* Terminate this thread. */
    pthread_exit(NULL);
}

/**
 * @brief denm_receiver_active
 * @param p_param
 * @return
 */
void *denm_receiver_active(void *p_param __attribute__((unused))) {

        printf("Starting DENM receiver\n");

    while(true == m_bIsThreadsActive) {

        sa_mngr_process_rx_denm();
    }

    printf("Stopping DENM receiver\n");

    /* Terminate this thread. */
    pthread_exit(NULL);
}

/**
 * @brief cam_receiver_active
 * @param p_param
 * @return
 */
void *cam_receiver_active(void *p_param __attribute__((unused))) {

    //pBtpCamHandler_ptr pBtpCamHandler; // TODO Why?

    printf("Starting CAM receiver\n");

    while(true == m_bIsThreadsActive) {

        sa_mngr_process_rx_cam();
    }

    printf("Stopping CAM receiver\n");

    /* Terminate this thread. */
    pthread_exit(NULL);
}

