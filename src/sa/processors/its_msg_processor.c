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

int32_t its_msg_processor_process_tx_pending_denms(fix_data_t *psPotiFixData) {

    while(true == its_msg_processor_is_tx_denm_pending()) {

        its_msg_processor_process_tx_denm(psPotiFixData);
    }

    // TODO Add getting results.
    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_process_tx_cam(fix_data_t *psPotiFixData) {

    CAM *psCam = NULL;

    if(false == its_msg_processor_pop_tx_cam_msg(&psCam)) {

        its_msg_processor_allocate_tx_cam_msg(&psCam);
    }

    if(NULL == psCam) {

        printf("Failed to allocate tx cam resources\n");
        return PROCEDURE_BUFFER_ERROR;
    }

    // ------------------------------------------------
    // ---------------- Publish Situation -------------
    // ------------------------------------------------

    //printf("Processing outgoing CAM message\n");

    return cam_mngr_process_tx(&g_sLocalStationInfo, psPotiFixData, psCam);
}

int32_t its_msg_processor_process_tx_denm(fix_data_t *psPotiFixData) {

    DENM *psDenm = NULL;

    if(false == its_msg_processor_pop_tx_denm_msg(&psDenm)) {

        printf("Pushing default DENM\n");
        its_msg_processor_allocate_tx_denm_msg(&psDenm);
    }

    if(NULL == psDenm) {

        printf("Failed to allocate denm tx resources\n");
        return PROCEDURE_BUFFER_ERROR;
    }

    // ------------------------------------------------
    // ---------------- Publish Situation -------------
    // ------------------------------------------------

    //printf("Processing outgoing DENM message\n");

    return denm_mngr_process_tx(&g_sLocalStationInfo, psPotiFixData, psDenm);
}

int32_t its_msg_processor_allocate_tx_cam_msg(CAM **p2sCam) {

    *p2sCam = &m_asCamTxRingBuffer[m_n32CamTxRingBufferIndex++];
    m_n32CamTxRingBufferIndex = m_n32CamTxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    memset(*p2sCam, 0, sizeof(CAM));

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_allocate_rx_cam_msg(CAM **p2sCam) {

    *p2sCam = &m_asCamRxRingBuffer[m_n32CamRxRingBufferIndex++];
    m_n32CamRxRingBufferIndex = m_n32CamRxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    memset(*p2sCam, 0, sizeof(CAM));

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_allocate_tx_denm_msg(DENM **p2sDenm) {

    *p2sDenm = &m_asDenmTxRingBuffer[m_n32DenmTxRingBufferIndex++];
    m_n32DenmTxRingBufferIndex = m_n32DenmTxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    memset(*p2sDenm, 0, sizeof(DENM));

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_allocate_rx_denm_msg(DENM **p2sDenm) {

    *p2sDenm = &m_asDenmRxRingBuffer[m_n32DenmRxRingBufferIndex++];
    m_n32DenmRxRingBufferIndex = m_n32DenmRxRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    memset(*p2sDenm, 0, sizeof(DENM));

    return PROCEDURE_SUCCESSFULL;
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
