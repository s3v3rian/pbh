#include "ring_buffer.h"

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

typedef struct SPoolDescriptor {

    bool m_bIsInUse;
    uint32_t m_un32CurrentAllocateIndex;
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

        m_asContainers[un32Index].m_bIsInUse = false;
        m_asContainers[un32Index].m_un32CurrentAllocateIndex = 0;
    }

    return PROCEDURE_SUCCESSFULL;
}

int32_t ring_buffer_container_init(const char *pchName, uint32_t un32ElementSize) {

    int32_t n32PoolIndex = INVALID_CONTAINER_ID;
    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS; n32Index++) {

        if(false == m_asContainers[n32Index].m_bIsInUse) {

            n32PoolIndex = n32Index;
            m_asContainers[n32Index].m_bIsInUse = true;
            m_asContainers[n32Index].m_un32ElementSize = un32ElementSize;
            m_asContainers[n32Index].m_sContainerData.m_n32Data = INVALID_CONTAINER_ELEMENT_ID;
            m_asContainers[n32Index].m_sContainerData.m_pchData = calloc(MAX_NUMBER_OF_CONTAINERS_ELEMENTS, un32ElementSize);
            break;
        }
    }

    printf("Creating Ring Buffer - %s, Size: %d, Pool ID: %d\n", pchName, un32ElementSize, n32PoolIndex);

    return n32PoolIndex;
}

int32_t ring_buffer_container_allocate(int32_t n32ContainerId, char **p2chElement) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("spsc pop operation, invalid container id\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    SPoolDescriptor *psPoolDescriptor = &m_asContainers[n32ContainerId];

    if(false == psPoolDescriptor->m_bIsInUse) {

        printf("Allocate operation failed, container %d is not in use\n", n32ContainerId);
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    printf("Allocating from ring buffer %d element %d\n", n32ContainerId, psPoolDescriptor->m_un32CurrentAllocateIndex);

    *p2chElement = psPoolDescriptor->m_sContainerData.m_pchData + psPoolDescriptor->m_un32ElementSize * psPoolDescriptor->m_un32CurrentAllocateIndex;

    /* Make sure we reset the data structure at least once. */
    memset(*p2chElement, 0, psPoolDescriptor->m_un32ElementSize);

    psPoolDescriptor->m_un32CurrentAllocateIndex = ((psPoolDescriptor->m_un32CurrentAllocateIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);

    return PROCEDURE_SUCCESSFULL;
}

int32_t ring_buffer_container_release(int32_t n32ContainerId) {

    if(0 > n32ContainerId
            || MAX_NUMBER_OF_CONTAINERS <= n32ContainerId) {

        printf("Invalid container id for spsc release operation\n");
        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    m_asContainers[n32ContainerId].m_bIsInUse = false;
    free(m_asContainers[n32ContainerId].m_sContainerData.m_pchData);

    return PROCEDURE_SUCCESSFULL;
}

int32_t ring_buffer_release() {

    for(int32_t n32Index = 0; n32Index < MAX_NUMBER_OF_CONTAINERS; n32Index++) {

        if(true == m_asContainers[n32Index].m_bIsInUse) {

            free(m_asContainers[n32Index].m_sContainerData.m_pchData);
            m_asContainers[n32Index].m_bIsInUse = false;
        }
    }

    return PROCEDURE_SUCCESSFULL;
}
