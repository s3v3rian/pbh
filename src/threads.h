#ifndef THREADS_H_
#define THREADS_H_

#include <signal.h>
#include <pthread.h>

#include "cam/cam_mngr.h"
#include "denm/denm_mngr.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Data type definition
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

// All threads.
pthread_t g_asThreads[MAX_NUMBER_OF_THREADS];

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

void error_catcher_active(int signal);
void exit_catcher_active(int signal);
void *poti_receiver_active(void *p_param __attribute__((unused)));
void *cam_receiver_active(void *p_param __attribute__((unused)));
void *denm_receiver_active(void *p_param __attribute__((unused)));
void *sa_processor_active(void *p_param __attribute__((unused)));
void *gui_active(void *p_param __attribute__((unused)));

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


