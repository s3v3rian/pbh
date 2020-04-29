#include "sa_mngr.h"

#include "lib/inc/error_code_user.h"

#include "common/containers/blocked_array_queue.h"
#include "common/containers/ring_buffer.h"
#include "common/utils/geo_utils.h"
#include "common/utils/print_utils.h"

#include "services/gps/gps_poti.h"

#include "sa/algs/alg_haversine.h"

/*
 *******************************************************************************
 * Private function signatures.
 *******************************************************************************
 */

static void sa_mngr_handle_msg(int32_t n32MsgId, char *pchMsgData);
static void sa_mngr_handle_cam(CAM *psCam);
static void sa_mngr_handle_denm(DENM *psDenm);
static void sa_mngr_handle_publish(fix_data_t *psPotiFixData);

/*
 *******************************************************************************
 * Private/Extern variables
 *******************************************************************************
 */

// Container ID's.
int32_t m_n32FusionThreadQueueId = INVALID_CONTAINER_ID;

int32_t m_n32CamRxRingBufferId = INVALID_CONTAINER_ID;
int32_t m_n32DenmRxRingBufferId = INVALID_CONTAINER_ID;
int32_t m_n32PotiRingBufferId = INVALID_CONTAINER_ID;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t sa_mngr_init() {

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    n32ProcedureResult = gps_poti_init();

    if(FALSE == IS_SUCCESS(n32ProcedureResult)) {

        printf("Cannot create POTI service: %s\n", ERROR_MSG(n32ProcedureResult));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    cam_mngr_init();

    if(FALSE == IS_SUCCESS(n32ProcedureResult)) {

        printf("Cannot init cam manager: %s\n", ERROR_MSG(n32ProcedureResult));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    denm_mngr_init();

    if(FALSE == IS_SUCCESS(n32ProcedureResult)) {

        printf("Cannot init denm manager: %s\n", ERROR_MSG(n32ProcedureResult));
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    m_n32FusionThreadQueueId = blocked_array_queue_container_init("sa_fusion_queue");

    m_n32CamRxRingBufferId = ring_buffer_container_init("cam_rx_ring_buffer", sizeof(CAM));
    m_n32DenmRxRingBufferId = ring_buffer_container_init("denm_rx_ring_buffer", sizeof(DENM));
    m_n32PotiRingBufferId = ring_buffer_container_init("poti_ring_buffer", sizeof(fix_data_t));

    if(0 > m_n32FusionThreadQueueId
            || 0 > m_n32CamRxRingBufferId
            || 0 > m_n32DenmRxRingBufferId) {

        printf("Cannot init sa containers\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    // Initialize station processor.
    if(PROCEDURE_SUCCESSFULL != g_fp_its_processor_init()) {

        printf("Cannot init station sa processor\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    return n32ProcedureResult;
}

void sa_mngr_process_fusion() {

    int32_t n32MsgId = 0;
    char *pchMsgData = NULL;

    blocked_array_queue_container_pop(m_n32FusionThreadQueueId, &n32MsgId, &pchMsgData);

    // Handle spurious wakeups.
    if(NULL == pchMsgData
            || INVALID_CONTAINER_ELEMENT_ID == n32MsgId) {

        return;
    }

    switch(n32MsgId) {

        case POTI_id:

            sa_mngr_handle_publish((fix_data_t*)pchMsgData);
            break;

        default:

            sa_mngr_handle_msg(n32MsgId, pchMsgData);
            break;
    }
}

void sa_mngr_process_poti() {

    fix_data_t *psPotiFixData = NULL;
    int32_t n32GnssUpdateCount = 0;
    int32_t n32ProcedureResult = 0;

    if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32PotiRingBufferId, (char**)(&psPotiFixData))) {

        printf("Failed to allocate poti resources\n");
        return;
    }

    n32ProcedureResult = gps_poti_get_fix_data(psPotiFixData);
    n32GnssUpdateCount++;

    if(g_sTxParameters.m_n32TxFrequencyIn10Hz > n32GnssUpdateCount) {

        return;
    }

    n32GnssUpdateCount = 0;

    /* Make sure the navigation information is valid. */
    if(psPotiFixData->status == FIX_STATUS_NA
                || psPotiFixData->mode == FIX_MODE_NA
                || psPotiFixData->mode == FIX_MODE_NO_FIX) {

        printf("Navigation information is invalid\n");
    }

    gps_poti_mngr_printf_poti(psPotiFixData);

    blocked_array_queue_container_push(m_n32FusionThreadQueueId, POTI_id, (char*)psPotiFixData);

    fflush(stdout);
}

void sa_mngr_process_cam() {

    CAM *psOutputCam = NULL;

    if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32CamRxRingBufferId, (char**)(&psOutputCam))) {

        printf("Failed to allocate rx cam resources\n");
        return;
    }

    if(PROCEDURE_SUCCESSFULL != cam_mngr_process_rx(psOutputCam)) {

        return;
    }

    cam_mngr_printf_cam(psOutputCam);

    blocked_array_queue_container_push(m_n32FusionThreadQueueId, CAM_Id, (char*)psOutputCam);
}

void sa_mngr_process_denm() {

    DENM *psOutputDenm = NULL;

    if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32DenmRxRingBufferId, (char**)(&psOutputDenm))) {

        printf("Failed to allocate rx denm resources\n");
        return;
    }


    if(PROCEDURE_SUCCESSFULL != denm_mngr_process_rx(psOutputDenm)) {

        return;
    }

    denm_mngr_printf_denm(psOutputDenm);

    blocked_array_queue_container_push(m_n32FusionThreadQueueId, DENM_Id, (char*)psOutputDenm);
}

void sa_mngr_release() {

    gps_poti_release();
    cam_mngr_release();
    denm_mngr_release();
}

static void sa_mngr_handle_msg(int32_t n32MsgId, char *pchMsgData) {

    switch(n32MsgId) {

        case DENM_Id:

            sa_mngr_handle_denm((DENM*)pchMsgData);
            break;

        case CAM_Id:

            sa_mngr_handle_cam((CAM*)pchMsgData);
            break;

        default:

            printf("SA_MNGR - Received unknown message data - %d\n", n32MsgId);
            break;
    }
}

static void sa_mngr_handle_cam(CAM *psCam) {

    SITSStationFusionData *psLocalStationData = sa_database_get_station_data(g_sLocalStationInfo.m_un32StationId);

    if(NULL == psLocalStationData) {

        printf("SA_MNGR - Failed to find local station id for computing haversine\n");
        return;
    }

    SITSStationFusionData *psRemoteStationData = sa_database_get_station_data(psCam->header.stationID);

    if(NULL == psRemoteStationData) {

        psRemoteStationData = sa_database_add_station_data(psCam->header.stationID);

        if(NULL == psRemoteStationData) {

            printf("SA_MNGR - Failed to find station %d fusion info\n", psCam->header.stationID);
            return;
        }

        // Update additional station info.
        psRemoteStationData->m_n32StationType = psCam->cam.camParameters.basicContainer.stationType;
    }

    //printf("Found station data of station %d\n", psCam->header.stationID);

    // --------------------------------------------------
    // ---------- Save Remote Station Data --------------
    // --------------------------------------------------

    // Update remote station LLA.
    psRemoteStationData->m_sCurrentLLAData.m_dLongitude = psCam->cam.camParameters.basicContainer.referencePosition.longitude / 10000000.0;
    psRemoteStationData->m_sCurrentLLAData.m_dLatitude = psCam->cam.camParameters.basicContainer.referencePosition.latitude / 10000000.0;

    // Update remote station ka.
    psRemoteStationData->m_dLastPotiTAI = psLocalStationData->m_dLastPotiTAI;

    // --------------------------------------------------
    // -------------- Compute Distance ------------------
    // --------------------------------------------------

    // Compute distance to target.
    double ndHaversine = alg_haversine_compute(&psLocalStationData->m_sCurrentLLAData, &psRemoteStationData->m_sCurrentLLAData);

    printf("SA_MNGR - Computed haversine value %f from station %d to station %d\n", ndHaversine, g_sLocalStationInfo.m_un32StationId, psCam->header.stationID);

    // --------------------------------------------------
    // ------------- Make Event Decision ----------------
    // --------------------------------------------------

    g_fp_its_processor_proccess_cam(psCam, psLocalStationData, psRemoteStationData);
}

static void sa_mngr_handle_denm(DENM *psDenm) {

    SITSStationFusionData *psLocalStationData = sa_database_get_station_data(g_sLocalStationInfo.m_un32StationId);

    if(NULL == psLocalStationData) {

        printf("SA_MNGR - Failed to find local station id for computing haversine\n");
        return;
    }

    SITSStationFusionData *psRemoteStationData = sa_database_get_station_data(psDenm->header.stationID);

    if(NULL == psRemoteStationData) {

        psRemoteStationData = sa_database_add_station_data(psDenm->header.stationID);

        if(NULL == psRemoteStationData) {

            printf("SA_MNGR - Failed to find station %d fusion info\n", psDenm->header.stationID);
            return;
        }

        // Update additional station info.
        psRemoteStationData->m_n32StationType = psDenm->denm.management.stationType;
    }

    //printf("SA_MNGR - Found station data of station %d\n", psDenm->header.stationID);

    // --------------------------------------------------
    // ------------- Make Event Decision ----------------
    // --------------------------------------------------

    g_fp_its_processor_proccess_denm(psDenm, psLocalStationData, psRemoteStationData);
}

static void sa_mngr_handle_publish(fix_data_t *psPotiFixData) {

    SITSStationFusionData *psStationData = sa_database_get_station_data(g_sLocalStationInfo.m_un32StationId);

    if(NULL == psStationData) {

        psStationData = sa_database_add_station_data(g_sLocalStationInfo.m_un32StationId);

        if(NULL == psStationData) {

            printf("SA_MNGR - Failed to find/add station %d fusion info\n", g_sLocalStationInfo.m_un32StationId);
            return;
        }
    }

    // --------------------------------------------------
    // ----------- Save Local Station Data --------------
    // --------------------------------------------------

    // Save Local LLA.
    psStationData->m_sCurrentLLAData.m_dLongitude = psPotiFixData->longitude;
    psStationData->m_sCurrentLLAData.m_dLatitude = psPotiFixData->latitude;

    // Save timestamp.
    psStationData->m_dLastPotiTAI = psPotiFixData->time.tai_since_2004;

    // ------------------------------------------------
    // ---------- Get New Data From Processor ---------
    // ------------------------------------------------

    CAM *psOutputCam = NULL;
    DENM *psOutputDenm = NULL;

    g_fp_its_processor_process_poti_cam(psPotiFixData, &psOutputCam);
    g_fp_its_processor_process_poti_denm(psPotiFixData, &psOutputDenm);

    if(NULL == psOutputCam
        || NULL == psOutputDenm) {

        printf("SA_MNGR - Failed to allocate tx resources\n");
        return;
    }

    // ------------------------------------------------
    // ---------------- Publish Situation -------------
    // ------------------------------------------------

    cam_mngr_process_tx(&g_sLocalStationInfo, psPotiFixData, psOutputCam);
    denm_mngr_process_tx(&g_sLocalStationInfo, psPotiFixData, psOutputDenm);

    // ----------------------------------------------------
    // ---------------- Post Processing -------------------
    // ----------------------------------------------------

#ifdef __SIMULATOR_ENABLED__

    if(true == g_sScenarioInfo.m_bIsGpsSimEnabled
            && 0 != g_sScenarioInfo.m_un32GpSimSyncId) {

        if(true == gps_poti_sim_is_paused()) {

            SITSStationFusionData *psStationData = sa_database_get_station_data(g_sScenarioInfo.m_un32GpSimSyncId);

            if(NULL != psStationData) {

                gps_poti_sim_pause_fix_data(false);
            }
        }
    }

#endif

}
