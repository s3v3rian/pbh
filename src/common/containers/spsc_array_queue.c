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

    _Atomic bool m_baIsInUse;
    _Atomic uint32_t m_un32aCurrentPushIndex;
    _Atomic uint32_t m_un32aCurrentPopIndex;

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

int32_t spsc_array_queue_init() {

    m_un32CurrentContainerInitIndex = 0;

    for(uint32_t un32Index = 0; un32Index < MAX_NUMBER_OF_CONTAINERS; un32Index++) {

        m_asContainers[un32Index].m_baIsInUse = false;
        m_asContainers[un32Index].m_un32aCurrentPushIndex = 0;
        m_asContainers[un32Index].m_un32aCurrentPopIndex = 0;
    }

    // Create queue elements array.
    m_psContainerArray = calloc(MAX_NUMBER_OF_CONTAINERS * MAX_NUMBER_OF_CONTAINERS_ELEMENTS, sizeof(SDataContainerElement));

    for(uint32_t un32Index = 0; un32Index < MAX_NUMBER_OF_CONTAINERS * MAX_NUMBER_OF_CONTAINERS_ELEMENTS; un32Index++) {

        m_psContainerArray[un32Index].m_n32Data = INVALID_CONTAINER_ELEMENT_ID;
        m_psContainerArray[un32Index].m_pchData = NULL;
    }

    return PROCEDURE_SUCCESSFULL;
}

int32_t spsc_array_queue_container_init(const char *pchName) {

    printf("Creating SPSC Array Queue - %s\n", pchName);

    int32_t n32QueueIndex = INVALID_CONTAINER_ID;
    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS; n32Index++) {

        if(false == m_asContainers[n32Index].m_baIsInUse) {

            m_asContainers[n32Index].m_baIsInUse = true;
            n32QueueIndex= n32Index;
            break;
        }
    }
    return n32QueueIndex;
}

int32_t spsc_array_queue_container_push(int32_t n32ContainerId, int32_t n32ElementId, char *pchElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("spsc push operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SQueueDescriptor *psQueueDescriptor = &m_asContainers[n32ContainerId];

    if(false == psQueueDescriptor->m_baIsInUse) {

        printf("spsc push operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SDataContainerElement *psQueueElement = m_psContainerArray + sizeof(SDataContainerElement) * ((n32ContainerId * MAX_NUMBER_OF_CONTAINERS_ELEMENTS) + psQueueDescriptor->m_un32aCurrentPushIndex);
    psQueueElement->m_n32Data = n32ElementId;
    psQueueElement->m_pchData = pchElement;
    psQueueDescriptor->m_un32aCurrentPushIndex = ((psQueueDescriptor->m_un32aCurrentPushIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}

int32_t spsc_array_queue_container_pop(int32_t n32ContainerId, int32_t *pn32ElementId, char **p2chElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("spsc pop operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SQueueDescriptor *psQueueDescriptor = &m_asContainers[n32ContainerId];

    if(false == psQueueDescriptor->m_baIsInUse) {

        printf("spsc pop operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SDataContainerElement *psQueueElement = m_psContainerArray + sizeof(SDataContainerElement) * ((n32ContainerId * MAX_NUMBER_OF_CONTAINERS_ELEMENTS) + psQueueDescriptor->m_un32aCurrentPopIndex);
    *pn32ElementId = psQueueElement->m_n32Data;
    *p2chElement = psQueueElement->m_pchData;

    psQueueElement->m_n32Data = INVALID_CONTAINER_ELEMENT_ID;
    psQueueElement->m_pchData = NULL;

    psQueueDescriptor->m_un32aCurrentPopIndex = ((psQueueDescriptor->m_un32aCurrentPopIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}

int32_t spsc_array_queue_container_release(int32_t n32ContainerId) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("Invalid container id for spsc release operation\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    m_asContainers[n32ContainerId].m_baIsInUse = false;

    return PROCEDURE_SUCCESSFULL;
}

int32_t spsc_array_queue_release() {

    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS; n32Index++) {

        m_asContainers[n32Index].m_baIsInUse = false;
    }

    free(m_psContainerArray);

    return PROCEDURE_SUCCESSFULL;
}
