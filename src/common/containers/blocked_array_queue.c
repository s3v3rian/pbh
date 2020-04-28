#include "blocked_array_queue.h"

#include <string.h>
#include <stdio.h>
#include <pthread.h>

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
    pthread_mutex_t m_Mutex;
    pthread_cond_t m_Conditon;

} SQueueDescriptor;

/*
 *******************************************************************************
 * Private variables
 *******************************************************************************
 */

uint32_t m_un32CurrentContainerInitIndex;
SQueueDescriptor m_asContainers[MAX_NUMBER_OF_CONTAINERS];
SDataContainerElement *m_psContainerArray;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t blocked_array_queue_init() {

    m_un32CurrentContainerInitIndex = 0;

    for(uint32_t un32Index = 0; un32Index < MAX_NUMBER_OF_CONTAINERS; un32Index++) {

        m_asContainers[un32Index].m_bIsInUse = false;
        m_asContainers[un32Index].m_un32CurrentPushIndex = 0;
        m_asContainers[un32Index].m_un32CurrentPopIndex = 0;
    }

    // Create queue elements array.
    m_psContainerArray = malloc(sizeof(SDataContainerElement) * MAX_NUMBER_OF_CONTAINERS * MAX_NUMBER_OF_CONTAINERS_ELEMENTS);

    for(uint32_t un32Index = 0; un32Index < MAX_NUMBER_OF_CONTAINERS * MAX_NUMBER_OF_CONTAINERS_ELEMENTS; un32Index++) {

        m_psContainerArray[un32Index].m_n32Data = INVALID_CONTAINER_ELEMENT_ID;
        m_psContainerArray[un32Index].m_pchData = NULL;
    }

    return PROCEDURE_SUCCESSFULL;
}

int32_t blocked_array_queue_container_init(const char *pchName) {

    printf("Creating SPSC Array Queue - %s\n", pchName);

    int32_t n32QueueIndex = INVALID_CONTAINER_ID;
    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS; n32Index++) {

        if(false == m_asContainers[n32Index].m_bIsInUse) {

            m_asContainers[n32Index].m_bIsInUse = true;
            n32QueueIndex= n32Index;
            break;
        }
    }
    return n32QueueIndex;
}

int32_t blocked_array_queue_container_push(int32_t n32ContainerId, int32_t n32ElementId, char *pchElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("array push operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SQueueDescriptor *psQueueDescriptor = &m_asContainers[n32ContainerId];

    if(false == psQueueDescriptor->m_bIsInUse) {

        printf("array push operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    pthread_mutex_lock(&psQueueDescriptor->m_Mutex);


    SDataContainerElement *psQueueElement = m_psContainerArray + sizeof(SDataContainerElement) * ((n32ContainerId * MAX_NUMBER_OF_CONTAINERS_ELEMENTS) + psQueueDescriptor->m_un32CurrentPushIndex);
    psQueueElement->m_n32Data = n32ElementId;
    psQueueElement->m_pchData = pchElement;
    psQueueDescriptor->m_un32CurrentPushIndex = ((psQueueDescriptor->m_un32CurrentPushIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);

    pthread_cond_signal(&psQueueDescriptor->m_Conditon);
    pthread_mutex_unlock(&psQueueDescriptor->m_Mutex);

    return PROCEDURE_SUCCESSFULL;
}

int32_t blocked_array_queue_container_pop(int32_t n32ContainerId, int32_t *pn32ElementId, char **p2chElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("array pop operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SQueueDescriptor *psQueueDescriptor = &m_asContainers[n32ContainerId];

    if(false == psQueueDescriptor->m_bIsInUse) {

        printf("array pop operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    pthread_mutex_lock(&psQueueDescriptor->m_Mutex);
    pthread_cond_wait(&psQueueDescriptor->m_Conditon, &psQueueDescriptor->m_Mutex);

    void *pvQueueElement = m_psContainerArray + ((n32ContainerId * MAX_NUMBER_OF_CONTAINERS_ELEMENTS) + psQueueDescriptor->m_un32CurrentPopIndex);

    if(NULL != pvQueueElement) {

        SDataContainerElement *psQueueElement = m_psContainerArray + sizeof(SDataContainerElement) * ((n32ContainerId * MAX_NUMBER_OF_CONTAINERS_ELEMENTS) + psQueueDescriptor->m_un32CurrentPopIndex);
        *pn32ElementId = psQueueElement->m_n32Data;
        *p2chElement = psQueueElement->m_pchData;

        psQueueElement->m_n32Data = INVALID_CONTAINER_ELEMENT_ID;
        psQueueElement->m_pchData = NULL;

        psQueueDescriptor->m_un32CurrentPopIndex = ((psQueueDescriptor->m_un32CurrentPopIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);
    }

    pthread_mutex_unlock(&psQueueDescriptor->m_Mutex);

    return PROCEDURE_SUCCESSFULL;
}

int32_t blocked_array_queue_container_release(int32_t n32ContainerId) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("Invalid container id for array release operation\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    m_asContainers[n32ContainerId].m_bIsInUse = false;

    return PROCEDURE_SUCCESSFULL;
}

int32_t blocked_array_queue_release() {

    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS; n32Index++) {

        m_asContainers[n32Index].m_bIsInUse = false;
    }

    free(m_psContainerArray);

    return PROCEDURE_SUCCESSFULL;
}
