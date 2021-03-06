#ifndef COMMON_CONTAINERS_ARRAY_QUEUE_H_
#define COMMON_CONTAINERS_ARRAY_QUEUE_H_

#include "globals.h"

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

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t array_queue_init();
int32_t array_queue_release();
int32_t array_queue_container_init(const char *pchName);
int32_t array_queue_container_release(int32_t n32ContainerId);
int32_t array_queue_container_push(int32_t n32ContainerId, int32_t n32ElementId, char *pchElement);
int32_t array_queue_container_pop(int32_t n32ContainerId, int32_t *pn32ElementId, char **p2chElement);
bool array_queue_is_container_empty(int32_t n32ContainerId);

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


