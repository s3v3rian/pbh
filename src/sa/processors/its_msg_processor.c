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

static CAM m_asCamTxRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];
static DENM m_asDenmTxRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];
static CAM m_asCamRxRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];
static DENM m_asDenmRxRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];

static int32_t m_n32CamTxRingBufferIndex = 0;
static int32_t m_n32DenmTxRingBufferIndex = 0;
static int32_t m_n32CamRxRingBufferIndex = 0;
static int32_t m_n32DenmRxRingBufferIndex = 0;

static int32_t m_n32CamQueueId = INVALID_CONTAINER_ID;
static int32_t m_n32DenmQueueId = INVALID_CONTAINER_ID;

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

bool its_msg_processor_init() {

    m_n32CamQueueId = array_queue_container_init("sa_its_processor_cam_queue");
    m_n32DenmQueueId = array_queue_container_init("sa_its_processor_denm_queue");

    if(0 > m_n32CamQueueId
            || 0 > m_n32DenmQueueId) {

        printf("Cannot init sa containers\n");
        return false;
    }

    return true;
}

bool its_msg_processor_allocate_tx_cam_msg(CAM **p2sCam) {

    *p2sCam = &m_asCamTxRingBuffer[m_n32CamTxRingBufferIndex++];
    m_n32CamTxRingBufferIndex = m_n32CamTxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    return true;
}

bool its_msg_processor_allocate_rx_cam_msg(CAM **p2sCam) {

    *p2sCam = &m_asCamRxRingBuffer[m_n32CamRxRingBufferIndex++];
    m_n32CamRxRingBufferIndex = m_n32CamRxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    return true;
}

bool its_msg_processor_allocate_tx_denm_msg(DENM **p2sDenm) {

    *p2sDenm = &m_asDenmTxRingBuffer[m_n32DenmTxRingBufferIndex++];
    m_n32DenmTxRingBufferIndex = m_n32DenmTxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    memset(*p2sDenm, 0, sizeof(DENM));

    return true;
}

bool its_msg_processor_allocate_rx_denm_msg(DENM **p2sDenm) {

    *p2sDenm = &m_asDenmRxRingBuffer[m_n32DenmRxRingBufferIndex++];
    m_n32DenmRxRingBufferIndex = m_n32DenmRxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    return true;
}

bool its_msg_processor_push_tx_cam_msg(CAM *psCam) {

    return (PROCEDURE_SUCCESSFULL == array_queue_container_push(m_n32CamQueueId, 0, (char*)psCam)) ? true : false;
}

bool its_msg_processor_pop_tx_cam_msg(CAM **p2sCam) {

    int32_t n32ElementId = 0;
    array_queue_container_pop(m_n32CamQueueId, &n32ElementId, (char**)p2sCam);

    return (*p2sCam == NULL) ? false : true;
}

bool its_msg_processor_push_tx_denm_msg(DENM *psDenm) {

    return (PROCEDURE_SUCCESSFULL == array_queue_container_push(m_n32DenmQueueId, 0, (char*)psDenm)) ? true : false;
}

bool its_msg_processor_pop_tx_denm_msg(DENM **p2sDenm) {

    int32_t n32ElementId = 0;
    array_queue_container_pop(m_n32DenmQueueId, &n32ElementId, (char**)p2sDenm);

    return (*p2sDenm == NULL) ? false : true;
}

bool its_msg_processor_is_tx_cam_pending() {

    return (true == array_queue_is_container_empty(m_n32CamQueueId)) ? false : true;
}

bool its_msg_processor_is_tx_denm_pending() {

    return (array_queue_is_container_empty(m_n32DenmQueueId)) ? false : true;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
