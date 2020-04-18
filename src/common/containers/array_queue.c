#include "array_queue.h"

#include <string.h>
#include <stdio.h>

/*
 *******************************************************************************
 * Private constant value definition
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private data type definition
 *******************************************************************************
 */

typedef struct SQueueDescriptor {

    bool m_bIsInUse;
    uint32_t m_un32CurrentPushIndex;
    uint32_t m_un32CurrentPopIndex;

} SQueueDescriptor;

/*
 *******************************************************************************
 * Private variables
 *******************************************************************************
 */

uint32_t m_un32CurrentQueueInitIndex;
SQueueDescriptor m_asQueues[MAX_NUMBER_OF_QUEUES];
void *m_pvQueueArray;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t array_queue_init() {

    m_un32CurrentQueueInitIndex = 0;

    memset(m_asQueues, 0, sizeof(SQueueDescriptor) * MAX_NUMBER_OF_QUEUES);

    // Create queue elements array.
    m_pvQueueArray = malloc(sizeof(void*) * MAX_NUMBER_OF_QUEUES * MAX_NUMBER_OF_QUEUE_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}

int32_t array_queue_container_init(const char *pchName) {

    printf("Creating SPSC Array Queue - %s\n", pchName);

    int32_t n32QueueIndex = INVALID_QUEUE_ID;
    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_QUEUES; n32Index++) {

        if(false == m_asQueues[n32Index].m_bIsInUse) {

            m_asQueues[n32Index].m_bIsInUse = true;
            n32QueueIndex= n32Index;
            break;
        }
    }
    return n32QueueIndex;
}

int32_t array_queue_container_push(int32_t n32ContainerId, void *pvElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_QUEUES <= n32ContainerId) {

        printf("array push operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SQueueDescriptor *psQueueDescriptor = &m_asQueues[n32ContainerId];

    if(false == psQueueDescriptor->m_bIsInUse) {

        printf("array push operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    void *pvQueueElement = m_pvQueueArray + ((n32ContainerId * MAX_NUMBER_OF_QUEUE_ELEMENTS) + psQueueDescriptor->m_un32CurrentPushIndex);
    pvQueueElement = pvElement;
    psQueueDescriptor->m_un32CurrentPushIndex = ((psQueueDescriptor->m_un32CurrentPushIndex + 1) % MAX_NUMBER_OF_QUEUE_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}

int32_t array_queue_container_pop(int32_t n32ContainerId, void **p2vElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_QUEUES <= n32ContainerId) {

        printf("array pop operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SQueueDescriptor *psQueueDescriptor = &m_asQueues[n32ContainerId];

    if(false == psQueueDescriptor->m_bIsInUse) {

        printf("array pop operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    void *pvQueueElement = m_pvQueueArray + ((n32ContainerId * MAX_NUMBER_OF_QUEUE_ELEMENTS) + psQueueDescriptor->m_un32CurrentPopIndex);
    *p2vElement = pvQueueElement;
    pvQueueElement = NULL;
    psQueueDescriptor->m_un32CurrentPopIndex = ((psQueueDescriptor->m_un32CurrentPopIndex + 1) % MAX_NUMBER_OF_QUEUE_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}

int32_t array_queue_container_release(int32_t n32ContainerId) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_QUEUES <= n32ContainerId) {

        printf("Invalid container id for array release operation\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    m_asQueues[n32ContainerId].m_bIsInUse = false;

    return PROCEDURE_SUCCESSFULL;
}

int32_t array_queue_release() {

    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_QUEUES; n32Index++) {

        m_asQueues[n32Index].m_bIsInUse = false;
    }

    free(m_pvQueueArray);

    return PROCEDURE_SUCCESSFULL;
}
