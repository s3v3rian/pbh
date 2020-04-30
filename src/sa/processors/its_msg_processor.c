#include "its_msg_processor.h"

#include "lib/inc/gn_public.h"

#include "common/containers/ring_buffer.h"
#include "common/containers/array_queue.h"

/*
 *******************************************************************************
 * Private function signatures
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private constant value definition
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private macros
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private/Extern data type definition
 *******************************************************************************
 */

CAM m_asCamTxRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];
DENM m_asDenmTxRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];
CAM m_asCamRxRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];
DENM m_asDenmRxRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];

int32_t m_n32CamTxRingBufferIndex = 0;
int32_t m_n32DenmTxRingBufferIndex = 0;
int32_t m_n32CamRxRingBufferIndex = 0;
int32_t m_n32DenmRxRingBufferIndex = 0;

int32_t m_n32CamQueueId = INVALID_CONTAINER_ID;
int32_t m_n32DenmQueueId = INVALID_CONTAINER_ID;

/*
 *******************************************************************************
 * Private/Extern variables
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t its_msg_processor_init() {

    m_n32DenmQueueId = array_queue_container_init("sa_its_processor_denm_queue");
    m_n32CamQueueId = array_queue_container_init("sa_its_processor_cam_queue");

    /*
    m_n32CamTxRingBufferId = ring_buffer_container_init("cam_tx_ring_buffer", sizeof(CAM));
    m_n32DenmTxRingBufferId = ring_buffer_container_init("denm_tx_ring_buffer", sizeof(DENM));

    if(0 > m_n32CamQueueId
            || 0 > m_n32DenmQueueId
            || 0 > m_n32CamTxRingBufferId
            || 0 > m_n32DenmTxRingBufferId) {

        printf("Cannot init sa containers\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }
    */

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_get_tx_cam_msg(CAM **p2sCam) {

    its_msg_processor_pop_cam_msg(p2sCam);

    if(NULL != *p2sCam) {

        return PROCEDURE_SUCCESSFULL;
    }

    *p2sCam = &m_asCamTxRingBuffer[m_n32CamTxRingBufferIndex++];
    m_n32CamTxRingBufferIndex = m_n32CamTxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    memset(*p2sCam, 0, sizeof(CAM));

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_get_tx_denm_msg(DENM **p2sDenm) {

    its_msg_processor_pop_denm_msg(p2sDenm);

    if(NULL != *p2sDenm) {

        return PROCEDURE_SUCCESSFULL;
    }

    *p2sDenm = &m_asDenmTxRingBuffer[m_n32DenmTxRingBufferIndex++];
    m_n32DenmTxRingBufferIndex = m_n32DenmTxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    memset(*p2sDenm, 0, sizeof(DENM));

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_get_rx_cam_msg(CAM **p2sCam) {

    *p2sCam = &m_asCamRxRingBuffer[m_n32CamRxRingBufferIndex++];
    m_n32CamRxRingBufferIndex = m_n32CamRxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    memset(*p2sCam, 0, sizeof(CAM));

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_get_rx_denm_msg(DENM **p2sDenm) {

    *p2sDenm = &m_asDenmRxRingBuffer[m_n32DenmRxRingBufferIndex++];
    m_n32DenmRxRingBufferIndex = m_n32DenmRxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    memset(*p2sDenm, 0, sizeof(DENM));

    return PROCEDURE_SUCCESSFULL;
}

void its_msg_processor_push_cam_msg(CAM *psCam) {

    array_queue_container_push(m_n32CamQueueId, 0, (char*)psCam);
}

void its_msg_processor_push_denm_msg(DENM *psDenm) {

    array_queue_container_push(m_n32DenmQueueId, 0, (char*)psDenm);
}

void its_msg_processor_pop_cam_msg(CAM **p2sCam) {

    int32_t n32ElementId = 0;
    array_queue_container_pop(m_n32CamQueueId, &n32ElementId, (char**)p2sCam);
}

void its_msg_processor_pop_denm_msg(DENM **p2sDenm) {

    int32_t n32ElementId = 0;
    array_queue_container_pop(m_n32DenmQueueId, &n32ElementId, (char**)p2sDenm);
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
