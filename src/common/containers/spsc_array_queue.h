#ifndef COMMON_CONTAINERS_SPSC_ARRAY_QUEUE_H_
#define COMMON_CONTAINERS_SPSC_ARRAY_QUEUE_H_

#include "globals.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

#define MAX_QUEUE_NAME_STRING_SIZE_IN_BYTES	20
#define MAX_NUMBER_OF_QUEUES			20
#define MAX_NUMBER_OF_QUEUE_ELEMENTS		100
#define INVALID_QUEUE_ID			-1

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

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

void spsc_init();
int32_t spsc_queue_init(const char *pchName);
int32_t spsc_queue_push(int32_t n32ContainerId, void *pvElement);
int32_t spsc_queue_pop(int32_t n32ContainerId, void **p2vElement);
int32_t spsc_queue_release(int32_t n32ContainerId);
void spsc_release();

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


