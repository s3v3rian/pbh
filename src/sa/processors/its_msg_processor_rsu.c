#include "its_msg_processor_rsu.h"

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

/*
 *******************************************************************************
 * Private/Extern variables
 *******************************************************************************
 */

static int32_t m_n32CamQueueId	= INVALID_CONTAINER_ID;
static int32_t m_n32DenmQueueId = INVALID_CONTAINER_ID;

static int32_t m_n32CamTxRingBufferId = INVALID_CONTAINER_ID;
static int32_t m_n32DenmTxRingBufferId = INVALID_CONTAINER_ID;

static bool m_bIsDoingDenmStatusUpdate;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t its_msg_processor_rsu_init() {

    m_n32CamQueueId = array_queue_container_init("sa_its_processor_cam_queue");
    m_n32DenmQueueId = array_queue_container_init("sa_its_processor_denm_queue");

    m_n32CamTxRingBufferId = ring_buffer_container_init("cam_tx_ring_buffer", sizeof(CAM));
    m_n32DenmTxRingBufferId = ring_buffer_container_init("denm_tx_ring_buffer", sizeof(DENM));

    if(0 > m_n32CamQueueId
            || 0 > m_n32DenmQueueId
            || 0 > m_n32CamTxRingBufferId
            || 0 > m_n32DenmTxRingBufferId) {

        printf("Cannot init sa containers\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    m_bIsDoingDenmStatusUpdate = false;

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_process_cam(CAM *psCam, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData) {

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_process_denm(DENM *psDenm, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData) {

    if(GN_ITS_STATION_ROAD_SIDE_UNIT == psRemoteStationData->m_n32StationType) {

        /*
         * If remote is a rsu then make decision on how to handle next denm status.
         */
    }

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_process_poti_cam(fix_data_t *psPotiFixData, CAM **p2sCam) {

    int32_t n32ElementId = 0;
    char *pchData = NULL;

    array_queue_container_pop(m_n32CamQueueId, &n32ElementId, &pchData);

    if(NULL == pchData) {

        printf("Allocating default cam element\n");

        if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32CamTxRingBufferId, &pchData)) {

            return PROCEDURE_BUFFER_ERROR;
        }
    }

    *p2sCam = (CAM*)(pchData);

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_process_poti_denm(fix_data_t *psPotiFixData, DENM **p2sDenm) {

    int32_t n32ElementId = 0;
    char *pchData = NULL;

    array_queue_container_pop(m_n32DenmQueueId, &n32ElementId, &pchData);

    if(NULL == pchData) {

        printf("Allocating default denm element\n");

        if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32DenmTxRingBufferId, &pchData)) {

            return PROCEDURE_BUFFER_ERROR;
        }
    }

    *p2sDenm = (DENM*)(pchData);

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_clear_cam() {

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_clear_denm() {

    return PROCEDURE_SUCCESSFULL;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
