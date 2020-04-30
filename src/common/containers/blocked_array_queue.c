#include "blocked_array_queue.h"

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

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

    pthread_mutex_t m_Mutex;
    pthread_cond_t m_ConditionEmpty;

    bool m_bIsInUse;
    int32_t m_n32CurrentPushIndex;
    int32_t m_n32CurrentPopIndex;

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
        m_asContainers[un32Index].m_n32CurrentPushIndex = 0;
        m_asContainers[un32Index].m_n32CurrentPopIndex = 0;
    }

    // Create queue elements array.
    m_psContainerArray = calloc(MAX_NUMBER_OF_CONTAINERS * MAX_NUMBER_OF_CONTAINERS_ELEMENTS, sizeof(SDataContainerElement));

    for(uint32_t un32Index = 0; un32Index < MAX_NUMBER_OF_CONTAINERS * MAX_NUMBER_OF_CONTAINERS_ELEMENTS; un32Index++) {

        m_psContainerArray[un32Index].m_n32Data = INVALID_CONTAINER_ELEMENT_ID;
        m_psContainerArray[un32Index].m_pchData = NULL;
    }

    return PROCEDURE_SUCCESSFULL;
}

int32_t blocked_array_queue_container_init(const char *pchName) {

    printf("Creating Blocked Array Queue - %s\n", pchName);

    int32_t n32QueueIndex = INVALID_CONTAINER_ID;
    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS; n32Index++) {

        if(false == m_asContainers[n32Index].m_bIsInUse) {

            m_asContainers[n32Index].m_bIsInUse = true;
            n32QueueIndex= n32Index;

            pthread_mutex_init(&m_asContainers[n32Index].m_Mutex, NULL);
            pthread_cond_init(&m_asContainers[n32Index].m_ConditionEmpty, NULL);

           // m_asContainers[n32Index].m_pSemEmpty = sem_open("/empty", O_CREAT, 0644, 10);
            //m_asContainers[n32Index].m_pSemFull = sem_open("/full", O_CREAT, 0644, 0);
            //m_asContainers[n32Index].m_pSemMutex = sem_open("/mutex", O_CREAT, 0644, 1);

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

    printf("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");

    //sem_wait(psQueueDescriptor->m_pSemEmpty);
    //sem_wait(psQueueDescriptor->m_pSemMutex);

    pthread_mutex_lock(&psQueueDescriptor->m_Mutex);

    printf("Locked and loaded\n");

    SDataContainerElement *psQueueElement = m_psContainerArray + sizeof(SDataContainerElement) * ((n32ContainerId * MAX_NUMBER_OF_CONTAINERS_ELEMENTS) + psQueueDescriptor->m_n32CurrentPushIndex);
    psQueueElement->m_n32Data = n32ElementId;
    psQueueElement->m_pchData = pchElement;
    psQueueDescriptor->m_n32CurrentPushIndex = ((psQueueDescriptor->m_n32CurrentPushIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);

    //sem_post(psQueueDescriptor->m_pSemMutex);
    //sem_post(psQueueDescriptor->m_pSemFull);

    pthread_cond_broadcast(&psQueueDescriptor->m_ConditionEmpty);
    printf("Finishing 2....\n");
    pthread_mutex_unlock(&psQueueDescriptor->m_Mutex);

    printf("Finishing....\n");

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

    //sem_wait(psQueueDescriptor->m_pSemFull);
    //sem_wait(psQueueDescriptor->m_pSemMutex);
    pthread_mutex_lock(&psQueueDescriptor->m_Mutex);
    pthread_cond_wait(&psQueueDescriptor->m_ConditionEmpty, &psQueueDescriptor->m_Mutex);

    printf("Popping an item\n");

    void *pvQueueElement = m_psContainerArray + ((n32ContainerId * MAX_NUMBER_OF_CONTAINERS_ELEMENTS) + psQueueDescriptor->m_n32CurrentPopIndex);

    if(NULL != pvQueueElement) {

        SDataContainerElement *psQueueElement = m_psContainerArray + sizeof(SDataContainerElement) * ((n32ContainerId * MAX_NUMBER_OF_CONTAINERS_ELEMENTS) + psQueueDescriptor->m_n32CurrentPopIndex);
        *pn32ElementId = psQueueElement->m_n32Data;
        *p2chElement = psQueueElement->m_pchData;

        psQueueElement->m_n32Data = INVALID_CONTAINER_ELEMENT_ID;
        psQueueElement->m_pchData = NULL;

        psQueueDescriptor->m_n32CurrentPopIndex = ((psQueueDescriptor->m_n32CurrentPopIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);
    }

   // sem_post(psQueueDescriptor->m_pSemMutex);
   // sem_post(psQueueDescriptor->m_pSemEmpty);

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
