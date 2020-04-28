#include "sa_mngr.h"

#include "lib/inc/error_code_user.h"

#include "common/containers/blocked_array_queue.h"
#include "common/containers/ring_buffer.h"
#include "common/utils/print_utils.h"

#include "services/gps/gps_poti.h"
#include "services/gps/nmea_infra.h"

#include "sa/sa_database.h"

#include "sa/algs/alg_haversine.h"

/*
 *******************************************************************************
 * Private/Extern variables
 *******************************************************************************
 */

// Container ID's.
int32_t m_n32FusionQueueId = INVALID_CONTAINER_ID;
int32_t m_n32CamTxRingBufferId = INVALID_CONTAINER_ID;
int32_t m_n32CamRxRingBufferId = INVALID_CONTAINER_ID;
int32_t m_n32DenmTxRingBufferId = INVALID_CONTAINER_ID;
int32_t m_n32DenmRxRingBufferId = INVALID_CONTAINER_ID;

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

    m_n32FusionQueueId = blocked_array_queue_container_init("sa_fusion_queue");
    m_n32CamTxRingBufferId = ring_buffer_container_init("cam_tx_ring_buffer", sizeof(CAM));
    m_n32CamRxRingBufferId = ring_buffer_container_init("cam_rx_ring_buffer", sizeof(CAM));
    m_n32DenmTxRingBufferId = ring_buffer_container_init("denm_tx_ring_buffer", sizeof(DENM));
    m_n32DenmRxRingBufferId = ring_buffer_container_init("denm_rx_ring_buffer", sizeof(DENM));

    if(0 > m_n32FusionQueueId
            || 0 > m_n32CamTxRingBufferId
            || 0 > m_n32CamRxRingBufferId
            || 0 > m_n32DenmTxRingBufferId
            || 0 > m_n32DenmRxRingBufferId) {

        printf("Cannot init sa containers\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    return n32ProcedureResult;
}

void sa_mngr_process_fusion() {

    int32_t n32MsgId = 0;
    char *pchMsgData = NULL;

    blocked_array_queue_container_pop(m_n32FusionQueueId, &n32MsgId, &pchMsgData);

    // Handle spurious wakeups.
    if(NULL == pchMsgData
            || INVALID_CONTAINER_ELEMENT_ID == n32MsgId) {

        return;
    }

    switch(n32MsgId) {

        case POTI_id:

            sa_mngr_publish((fix_data_t*)pchMsgData);
            break;

        default:

            sa_mngr_update_db(n32MsgId, pchMsgData);
            break;
    }
}

void sa_mngr_process_poti() {

    fix_data_t *psPotiFixData = gps_poti_allocate_buffer();
    int32_t n32GnssUpdateCount = 0;
    int32_t n32ProcedureResult = 0;

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

    sa_mngr_output_poti(psPotiFixData);

    blocked_array_queue_container_push(m_n32FusionQueueId, POTI_id, (char*)psPotiFixData);

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

    sa_mngr_output_cam(psOutputCam);

    blocked_array_queue_container_push(m_n32FusionQueueId, CAM_Id, (char*)psOutputCam);
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

    sa_mngr_output_denm(psOutputDenm);

    blocked_array_queue_container_push(m_n32FusionQueueId, DENM_Id, (char*)psOutputDenm);
}

void sa_mngr_release() {

    gps_poti_release();
    cam_mngr_release();
    denm_mngr_release();
}

static void sa_mngr_update_db(int32_t n32MsgId, char *pchMsgData) {

    switch(n32MsgId) {

        case DENM_Id:

            sa_mngr_update_db_denm((DENM*)pchMsgData);
            break;

        case CAM_Id:

            sa_mngr_update_db_cam((CAM*)pchMsgData);
            break;

        case SPATEM_Id:

            break;

        case MAPEM_Id:

            break;

        case RTCMEM_Id:

            break;

        default:

            printf("SA_FUSION - Received unknown message data - %d\n", n32MsgId);
            break;
    }
}

static void sa_mngr_update_db_cam(CAM *psCam) {

    SStationFusionData *psStationData = sa_database_get_station_data(psCam->header.stationID);

    if(NULL == psStationData) {

        psStationData = sa_database_add_station_data(psCam->header.stationID);

        if(NULL == psStationData) {

            printf("SA_FUSION - Failed to find station %d fusion info\n", psCam->header.stationID);
            return;
        }
    }

    //printf("Found station data of station %d\n", psCam->header.stationID);

    // Update station data.
    psStationData->m_sCurrentLLAData.m_dLongitude = psCam->cam.camParameters.basicContainer.referencePosition.longitude / 10000000.0;
    psStationData->m_sCurrentLLAData.m_dLatitude = psCam->cam.camParameters.basicContainer.referencePosition.latitude / 10000000.0;

    SStationFusionData *psLocalStationData = sa_database_get_station_data(g_sLocalStationInfo.m_un32StationId);

    if(NULL == psLocalStationData) {

        printf("SA_FUSION - Failed to find local station id for computing haversine\n");
        return;
    }

    // Compute distance to target.
    double ndHaversine = alg_haversine_compute(&psLocalStationData->m_sCurrentLLAData, &psStationData->m_sCurrentLLAData);

    printf("SA_FUSION - Computed haversine value %f from station %d to station %d\n", ndHaversine, g_sLocalStationInfo.m_un32StationId, psCam->header.stationID);
}

static void sa_mngr_update_db_denm(DENM *psDenm) {

    SStationFusionData *psStationData = sa_database_get_station_data(psDenm->header.stationID);

    if(NULL == psStationData) {

        psStationData = sa_database_add_station_data(psDenm->header.stationID);

        if(NULL == psStationData) {

            printf("SA_FUSION - Failed to find station %d fusion info\n", psDenm->header.stationID);
            return;
        }
    }

    //printf("SA_FUSION - Found station data of station %d\n", psDenm->header.stationID);
}

static void sa_mngr_publish(fix_data_t *psPotiFixData) {

    // ------------------------------------------------
    // ---------------- Save Local Info ---------------
    // ------------------------------------------------

    SStationFusionData *psStationData = sa_database_get_station_data(g_sLocalStationInfo.m_un32StationId);

    if(NULL == psStationData) {

        psStationData = sa_database_add_station_data(g_sLocalStationInfo.m_un32StationId);

        if(NULL == psStationData) {

            printf("SA_FUSION - Failed to find/add station %d fusion info\n", g_sLocalStationInfo.m_un32StationId);
            return;
        }
    }

    psStationData->m_sCurrentLLAData.m_dLongitude = psPotiFixData->longitude;
    psStationData->m_sCurrentLLAData.m_dLatitude = psPotiFixData->latitude;

    // ------------------------------------------------
    // --------------- Allocate Resources -------------
    // ------------------------------------------------

    CAM *psOutputCam = NULL;
    DENM *psOutputDenm = NULL;

    if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32CamTxRingBufferId, (char**)(&psOutputCam))
        || PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32DenmTxRingBufferId, (char**)(&psOutputDenm))) {

        printf("SA_FUSION - Failed to allocate tx resources\n");
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

            SStationFusionData *psStationData = sa_database_get_station_data(g_sScenarioInfo.m_un32GpSimSyncId);

            if(NULL != psStationData) {

                gps_poti_sim_pause_fix_data(false);
            }
        }
    }

#endif

}

static int32_t sa_mngr_output_poti(fix_data_t *psPotiFixData) {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    // Convert POTI info to NMEA data.
    SNmeaGgaData sGgaData;
    SNmeaRmcData sRmcData;
    char achNmeaSentence[300];
    int32_t n32SentenceSize = 0;

    memset(&sGgaData, 0, sizeof(sGgaData));
    memset(&sRmcData, 0, sizeof(sRmcData));

    // Get data from POTI.
    nmea_get_gga_data(psPotiFixData, &sGgaData);
    nmea_get_rmc_data(psPotiFixData, &sRmcData);

    n32SentenceSize = nmea_build_gga_msg(&sGgaData, achNmeaSentence);

    // Build and send NMEA sentence.
    n32Result = g_fpBoundaryWriter(achNmeaSentence, n32SentenceSize, g_sLocalStationInfo.m_un32StationId);

    // Build and send NMEA sentence.
    n32SentenceSize = nmea_build_rmc_msg(&sRmcData, achNmeaSentence);
    n32Result &= g_fpBoundaryWriter(achNmeaSentence, n32SentenceSize, g_sLocalStationInfo.m_un32StationId);

    return n32Result;
}

int32_t sa_mngr_output_cam(CAM *psCam) {

    SNmeaRmcData sRmcData;
    char achSentence[300];
    int32_t n32SentenceSize = 0;

    memset(&sRmcData, 0, sizeof(sRmcData));

    nmea_get_rmc_Data(&sRmcData);

    sRmcData.m_dLatitude = psCam->cam.camParameters.basicContainer.referencePosition.latitude;
    sRmcData.m_dLatitude = sRmcData.m_dLatitude / 10000000.0;
    sRmcData.m_dLatitude = nmea_convert_decimal_degress_to_degrees_minutes(sRmcData.m_dLatitude);

    sRmcData.m_dLongitude = psCam->cam.camParameters.basicContainer.referencePosition.longitude;
    sRmcData.m_dLongitude = sRmcData.m_dLongitude / 10000000.0;
    sRmcData.m_dLongitude = nmea_convert_decimal_degress_to_degrees_minutes(sRmcData.m_dLongitude);

    sRmcData.m_dVelcoityInKnots = psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedValue * 1.944;

    n32SentenceSize = nmea_build_rmc_msg(&sRmcData, achSentence);
    return g_fpBoundaryWriter(achSentence, n32SentenceSize, psCam->header.stationID);
}

int32_t sa_mngr_output_denm(DENM *psDenm) {

    int32_t n32Result = PROCEDURE_SUCCESSFULL;

    char achSentence[300];

    if(TRUE == psDenm->denm.situation_option) {

        int32_t n32SentenceSize = denm_mngr_sprintf_situation(achSentence, &psDenm->denm.situation);
        n32Result = g_fpBoundaryWriter(achSentence, n32SentenceSize, psDenm->header.stationID);
    }

    if(TRUE == psDenm->denm.alacarte.stationaryVehicle_option) {

        int32_t n32SentenceSize = denm_mngr_sprintf_stationary(achSentence, &psDenm->denm.alacarte.stationaryVehicle);
        n32Result = g_fpBoundaryWriter(achSentence, n32SentenceSize, psDenm->header.stationID);
    }

    return n32Result;
}
