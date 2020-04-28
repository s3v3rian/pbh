#include "ring_buffer.h"

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

typedef struct SPoolDescriptor {

    _Atomic bool m_baIsInUse;
    _Atomic uint32_t m_un32aCurrentAllocateIndex;
    uint32_t m_un32ElementSize;
    SDataContainerElement m_sContainerData;

} SPoolDescriptor;

/*
 *******************************************************************************
 * Private variables
 *******************************************************************************
 */

uint32_t m_un32CurrentContainerInitIndex;
SPoolDescriptor m_asContainers[MAX_NUMBER_OF_CONTAINERS];

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t ring_buffer_init() {

    m_un32CurrentContainerInitIndex = 0;

    for(uint32_t un32Index = 0; un32Index < MAX_NUMBER_OF_CONTAINERS; un32Index++) {

        m_asContainers[un32Index].m_baIsInUse = false;
        m_asContainers[un32Index].m_un32aCurrentAllocateIndex = 0;
    }

    return PROCEDURE_SUCCESSFULL;
}

int32_t ring_buffer_container_init(const char *pchName, uint32_t un32ElementSize) {

    printf("Creating Ring Buffer - %s\n", pchName);

    int32_t n32PoolIndex = INVALID_CONTAINER_ID;
    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS; n32Index++) {

        if(false == m_asContainers[n32Index].m_baIsInUse) {

            m_asContainers[n32Index].m_baIsInUse = true;
            m_asContainers[n32Index].m_un32ElementSize = un32ElementSize;
            n32PoolIndex= n32Index;

            for(uint32_t un32Index = 0; un32Index < MAX_NUMBER_OF_CONTAINERS_ELEMENTS; un32Index++) {

                m_asContainers[n32Index].m_sContainerData.m_n32Data = INVALID_CONTAINER_ELEMENT_ID;
                m_asContainers[n32Index].m_sContainerData.m_pchData = malloc(un32ElementSize * MAX_NUMBER_OF_CONTAINERS_ELEMENTS);
            }
            break;
        }
    }
    return n32PoolIndex;
}

/*
int32_t ring_buffer_container_allocate(int32_t n32ContainerId, int32_t n32ElementId, char *pchElement) {

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

    SArrayPointerElement *psQueueElement = m_psContainerArray + sizeof(SArrayPointerElement) * ((n32ContainerId * MAX_NUMBER_OF_CONTAINERS_ELEMENTS) + psQueueDescriptor->m_un32aCurrentAllocateIndex);
    psQueueElement->n32Data = n32ElementId;
    psQueueElement->m_pchData = pchElement;
    psQueueDescriptor->m_un32aCurrentAllocateIndex = ((psQueueDescriptor->m_un32aCurrentAllocateIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}
*/

int32_t ring_buffer_container_allocate(int32_t n32ContainerId, char **p2chElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("spsc pop operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SPoolDescriptor *psPoolDescriptor = &m_asContainers[n32ContainerId];

    if(false == psPoolDescriptor->m_baIsInUse) {

        printf("spsc pop operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    *p2chElement = psPoolDescriptor->m_sContainerData.m_pchData + psPoolDescriptor->m_un32ElementSize * psPoolDescriptor->m_un32aCurrentAllocateIndex;

    psPoolDescriptor->m_un32aCurrentAllocateIndex = ((psPoolDescriptor->m_un32aCurrentAllocateIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}

int32_t ring_buffer_container_release(int32_t n32ContainerId) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("Invalid container id for spsc release operation\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    m_asContainers[n32ContainerId].m_baIsInUse = false;
    free(m_asContainers[n32ContainerId].m_sContainerData.m_pchData);

    return PROCEDURE_SUCCESSFULL;
}

int32_t ring_buffer_release() {

    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS; n32Index++) {

        m_asContainers[n32Index].m_baIsInUse = false;
    }

    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS * MAX_NUMBER_OF_CONTAINERS_ELEMENTS; n32Index++) {

        if(true == m_asContainers[n32Index].m_baIsInUse) {

            free(m_asContainers[n32Index].m_sContainerData.m_pchData);
        }
    }

    return PROCEDURE_SUCCESSFULL;
}
