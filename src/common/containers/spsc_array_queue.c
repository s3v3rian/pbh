#include "spsc_array_queue.h"

#include <string.h>
#include <stdio.h>
#include <stdatomic.h>

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

    _Atomic int32_t m_un32aIsInUse;
    _Atomic uint32_t m_un32aCurrentPushIndex;
    _Atomic uint32_t m_un32aCurrentPopIndex;

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

void spsc_init() {

    m_un32CurrentQueueInitIndex = 0;

    memset(m_asQueues, 0, sizeof(SQueueDescriptor) * MAX_NUMBER_OF_QUEUES);

    // Create queue elements array.
    m_pvQueueArray = malloc(sizeof(void*) * MAX_NUMBER_OF_QUEUES * MAX_NUMBER_OF_QUEUE_ELEMENTS);
}

int32_t spsc_queue_init(const char *pchName) {

    printf("Creating SPSC Array Queue - %s\n", pchName);

    int32_t n32QueueIndex = INVALID_QUEUE_ID;
    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_QUEUES; n32Index++) {

        if(BOOLEAN_FALSE == m_asQueues[n32Index].m_un32aIsInUse) {

            m_asQueues[n32Index].m_un32aIsInUse = BOOLEAN_TRUE;
            n32QueueIndex= n32Index;
            break;
        }
    }
    return n32QueueIndex;
}

int32_t spsc_queue_push(int32_t n32ContainerId, void *pvElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_QUEUES <= n32ContainerId) {

        printf("spsc push operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SQueueDescriptor *psQueueDescriptor = &m_asQueues[n32ContainerId];

    if(BOOLEAN_FALSE == psQueueDescriptor->m_un32aIsInUse) {

        printf("spsc push operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    void *pvQueueElement = m_pvQueueArray + ((n32ContainerId * MAX_NUMBER_OF_QUEUE_ELEMENTS) + psQueueDescriptor->m_un32aCurrentPushIndex);
    pvQueueElement = pvElement;
    psQueueDescriptor->m_un32aCurrentPushIndex = ((psQueueDescriptor->m_un32aCurrentPushIndex + 1) % MAX_NUMBER_OF_QUEUE_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}

int32_t spsc_queue_pop(int32_t n32ContainerId, void **p2vElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_QUEUES <= n32ContainerId) {

        printf("spsc pop operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SQueueDescriptor *psQueueDescriptor = &m_asQueues[n32ContainerId];

    if(BOOLEAN_FALSE == psQueueDescriptor->m_un32aIsInUse) {

        printf("spsc pop operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    void *pvQueueElement = m_pvQueueArray + ((n32ContainerId * MAX_NUMBER_OF_QUEUE_ELEMENTS) + psQueueDescriptor->m_un32aCurrentPopIndex);
    *p2vElement = pvQueueElement;
    pvQueueElement = NULL;
    psQueueDescriptor->m_un32aCurrentPopIndex = ((psQueueDescriptor->m_un32aCurrentPopIndex + 1) % MAX_NUMBER_OF_QUEUE_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}

int32_t spsc_queue_release(int32_t n32ContainerId) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_QUEUES <= n32ContainerId) {

        printf("Invalid container id for spsc release operation\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    m_asQueues[n32ContainerId].m_un32aIsInUse = BOOLEAN_FALSE;

    return PROCEDURE_SUCCESSFULL;
}

void spsc_release() {

    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_QUEUES; n32Index++) {

        m_asQueues[n32Index].m_un32aIsInUse = BOOLEAN_FALSE;
    }

    free(m_pvQueueArray);
}
