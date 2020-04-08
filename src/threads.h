#ifndef THREADS_H_
#define THREADS_H_

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
static pthread_t g_asThreads[8];

// Sender threads BTP handlers.
static btp_handler_ptr g_pBtpDenmHandler;

// POTI service handler.
static poti_service_t *g_psPotiHandler = NULL;

/*
 *******************************************************************************
 * Public activetions
 *******************************************************************************
 */

void *main_sender_active(void *p_param __attribute__((unused)));
void *cam_receiver_active(void *p_param __attribute__((unused)));
void *denm_receiver_active(void *p_param __attribute__((unused)));
void *sa_proc_active(void *p_param __attribute__((unused)));

/*
 *******************************************************************************
 * Private activetions
 *******************************************************************************
 */

#endif


