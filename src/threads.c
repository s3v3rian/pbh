#include "threads.h"

#include <time.h>
#include <unistd.h>

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
 * @brief error_catcher_active
 * @param signal
 */
void error_catcher_active(int signal) {

    printf("Caught error signal - %d\n", signal);

    m_bIsThreadsActive = false;

    exit(1);
}

/**
 * @brief exit_catcher_active
 * @param signal
 */
void exit_catcher_active(int signal) {

    printf("Caught exit signal - %d\n", signal);

    m_bIsThreadsActive = false;

    usleep(100000);

    exit(0);
}

/**
 * @brief main_sender_active
 * @param p_param
 * @return
 */
void *poti_receiver_active(void *p_param __attribute__((unused))) {

    printf("Starting POTI receiver\n");

    pthread_t tThisThread = pthread_self();

    struct sched_param sSchedParams;

    sSchedParams.sched_priority = 9;
    sched_setparam(tThisThread, &sSchedParams);
    sched_setscheduler(tThisThread, SCHED_FIFO, &sSchedParams);

    // -------------------------------------------------
    // --------------- Start POTI Loop -----------------
    // -------------------------------------------------

    while(true == m_bIsThreadsActive) {

        sa_mngr_process_poti();
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
 * @brief cam_receiver_active
 * @param p_param
 * @return
 */
void *cam_receiver_active(void *p_param __attribute__((unused))) {

    printf("Starting CAM receiver\n");

    pthread_t tThisThread = pthread_self();

    struct sched_param sSchedParams;

    sSchedParams.sched_priority = 7;
    sched_setparam(tThisThread, &sSchedParams);
    sched_setscheduler(tThisThread, SCHED_FIFO, &sSchedParams);

    while(true == m_bIsThreadsActive) {

        sa_mngr_process_cam();
    }

    printf("Stopping CAM receiver\n");

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

    pthread_t tThisThread = pthread_self();

    struct sched_param sSchedParams;

    sSchedParams.sched_priority = 7;
    sched_setparam(tThisThread, &sSchedParams);
    sched_setscheduler(tThisThread, SCHED_FIFO, &sSchedParams);

    while(true == m_bIsThreadsActive) {

        sa_mngr_process_denm();
    }

    printf("Stopping DENM receiver\n");

    /* Terminate this thread. */
    pthread_exit(NULL);
}

/**
 * @brief sa_processor_active
 * @param p_param
 * @return
 */
void *sa_processor_active(void *p_param __attribute__((unused))) {

    printf("Starting SA processor\n");

    pthread_t tThisThread = pthread_self();

    struct sched_param sSchedParams;

    sSchedParams.sched_priority = 8;
    sched_setparam(tThisThread, &sSchedParams);
    sched_setscheduler(tThisThread, SCHED_FIFO, &sSchedParams);

    while(true == m_bIsThreadsActive) {

        sa_mngr_process_fusion();
    }

    printf("Stopping SA processor\n");

    /* Terminate this thread. */
    pthread_exit(NULL);
}
