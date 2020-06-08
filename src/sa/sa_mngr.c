#include "sa_mngr.h"

#include <unistd.h>

#include "lib/inc/error_code_user.h"

#include "common/containers/spsc_array_queue.h"
#include "common/containers/ring_buffer.h"
#include "common/utils/geo_utils.h"
#include "common/utils/print_utils.h"
#include "common/sa_database.h"

#include "boundary/boundary_writer.h"

#include "services/gps/gps_poti.h"

#include "sa/processors/its_msg_processor.h"

#include "sa/algs/alg_haversine.h"

#include "sim/sim_mngr.h"

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
int32_t m_n32GnssUpdateCount = 0;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t sa_mngr_init() {

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    n32ProcedureResult = gps_poti_init();

    if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

        printf("Cannot create POTI service\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    n32ProcedureResult = cam_mngr_init();

    if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

        printf("Cannot init cam manager\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    n32ProcedureResult = denm_mngr_init();

    if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

        printf("Cannot init denm manager\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    m_n32FusionThreadQueueId = spsc_array_queue_container_init("sa_fusion_queue");

    // Initialize station processor.
    if(false == g_fp_its_processor_init()) {

        printf("Cannot init station sa processor\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    return PROCEDURE_SUCCESSFULL;
}

void sa_mngr_release() {

    gps_poti_release();
    cam_mngr_release();
    denm_mngr_release();
}

void sa_mngr_process_fusion() {

    int32_t n32MsgId = INVALID_CONTAINER_ELEMENT_ID;
    char *pchMsgData = NULL;

    //printf("Trying to pop data for fusion\n");

    usleep(1000);

    spsc_array_queue_container_pop(m_n32FusionThreadQueueId, &n32MsgId, &pchMsgData);

    // Handle spurious wakeups.
    if(NULL == pchMsgData
            || INVALID_CONTAINER_ELEMENT_ID == n32MsgId) {

        return;
    }

    //printf("------------ Handling new data for fusion ------------\n");

    // Read latest data from the boundary controller.
    g_fp_access_host_controller();

    // Process new message.
    switch(n32MsgId) {

        case POTI_id:

            sa_mngr_handle_publish((fix_data_t*)pchMsgData);
            break;

        default:

            sa_mngr_handle_msg(n32MsgId, pchMsgData);
            break;
    }

    fflush(stdout);
}

void sa_mngr_process_poti() {

    fix_data_t *psPotiFixData = NULL;
    int32_t n32ProcedureResult = 0;

    //printf("Processing received POTI data...\n");

    n32ProcedureResult = gps_poti_get_fix_data(&psPotiFixData);

    if(g_sLocalStationInfo.m_sParameters.m_n32TxFrequencyIn10Hz > m_n32GnssUpdateCount) {

        m_n32GnssUpdateCount++;
        return;
    }

    m_n32GnssUpdateCount = 0;

    /* Make sure the navigation information is valid. */
    if(psPotiFixData->status == FIX_STATUS_NA
                || psPotiFixData->mode == FIX_MODE_NA
                || psPotiFixData->mode == FIX_MODE_NO_FIX) {

        printf("Navigation information is invalid\n");
        return;
    }

#if (__EN_SIMULATOR_FEATURE__)

    //printf("Before Simulator GPS fix data update - TAI: %f, LAT: %f, LON: %f, ALT: %f\n", psPotiFixData->time.tai_since_2004, psPotiFixData->latitude, psPotiFixData->longitude, psPotiFixData->altitude);

    sim_mngr_process_fix_data(psPotiFixData);

    //printf("After Simulator GPS fix data update - TAI: %f, LAT: %f, LON: %f, ALT: %f\n", psPotiFixData->time.tai_since_2004,  psPotiFixData->latitude, psPotiFixData->longitude, psPotiFixData->altitude);

#endif

    // ------------------------------------------------
    // ------------ Process Pending TX ----------------
    // ------------------------------------------------

    CAM *psCam = NULL;
    DENM *psDenm = NULL;

    /*
    while(true == its_msg_processor_pop_tx_denm_msg(&psDenm)) {

        denm_mngr_process_tx(&g_sLocalStationInfo, psPotiFixData, psDenm);
    }
    */

    while(true == its_msg_processor_pop_tx_cam_msg(&psCam)) {

        cam_mngr_process_tx(&g_sLocalStationInfo, psPotiFixData, psCam);
        psCam = NULL;
    }

    g_fp_write_to_boundary_poti(psPotiFixData);

    //printf("Pushing new POTI data to SA\n");

    spsc_array_queue_container_push(m_n32FusionThreadQueueId, POTI_id, (char*)psPotiFixData);

    //printf("Finished processing received POTI data\n");
}

void sa_mngr_process_cam() {

    CAM *psOutputCam = NULL;

    if(false == its_msg_processor_allocate_rx_cam_msg(&psOutputCam)) {

        printf("Failed to allocate rx cam resources\n");
        return;
    }

    if(PROCEDURE_SUCCESSFULL != cam_mngr_process_rx(psOutputCam)) {

        printf("Failed to process received cam\n");
        return;
    }

    g_fp_write_to_boundary_cam(psOutputCam);

    //printf("Inserting new CAM for rx processing\n");

    spsc_array_queue_container_push(m_n32FusionThreadQueueId, CAM_Id, (char*)psOutputCam);
}

void sa_mngr_process_denm() {

    DENM *psOutputDenm = NULL;

    if(false == its_msg_processor_allocate_rx_denm_msg(&psOutputDenm)) {

        printf("Failed to allocate rx denm resources\n");
        return;
    }


    if(PROCEDURE_SUCCESSFULL != denm_mngr_process_rx(psOutputDenm)) {

        printf("Failed to process received denm\n");
        return;
    }

    g_fp_write_to_boundary_denm(psOutputDenm);

    //printf("Inserting new DENM for rx processing\n");

    spsc_array_queue_container_push(m_n32FusionThreadQueueId, DENM_Id, (char*)psOutputDenm);
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

    //printf("Handling received CAM message from %d\n", psCam->header.stationID);

    SStationFullFusionData *psLocalFusionData = sa_database_get_station_data(g_sLocalStationInfo.m_un32StationId);

    if(NULL == psLocalFusionData) {

        printf("SA_MNGR - Failed to find local station id for computing haversine\n");
        return;
    }

    SStationFullFusionData *psRemoteFusionData = sa_database_get_station_data(psCam->header.stationID);

    if(NULL == psRemoteFusionData) {

        psRemoteFusionData = sa_database_add_station_data(psCam->header.stationID);

        if(NULL == psRemoteFusionData) {

            printf("SA_MNGR - Failed to find station %d fusion info\n", psCam->header.stationID);
            return;
        }

        // Update additional station info.
        psRemoteFusionData->m_n32StationType = psCam->cam.camParameters.basicContainer.stationType;
    }

    //printf("Found station data of station %d\n", psCam->header.stationID);

    // --------------------------------------------------
    // ---------- Save Remote Station Data --------------
    // --------------------------------------------------

    // Update remote station LLA.
    psRemoteFusionData->m_sDistanceData.m_dLongitude = psCam->cam.camParameters.basicContainer.referencePosition.longitude / 10000000.0;
    psRemoteFusionData->m_sDistanceData.m_dLatitude = psCam->cam.camParameters.basicContainer.referencePosition.latitude / 10000000.0;

    // Update remote station ka.
    psRemoteFusionData->m_dLastPotiTAI = psLocalFusionData->m_dLastPotiTAI;

    // --------------------------------------------------
    // -------------- Compute Distance ------------------
    // --------------------------------------------------

    // Compute distance to target.
    psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters = alg_haversine_compute(&psLocalFusionData->m_sDistanceData, &psRemoteFusionData->m_sDistanceData);

    // Compute bearing.
    psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees = geodesic_calculate_bearing(psRemoteFusionData->m_sDistanceData.m_dLatitude, psRemoteFusionData->m_sDistanceData.m_dLongitude, psLocalFusionData->m_sDistanceData.m_dLatitude, psLocalFusionData->m_sDistanceData.m_dLongitude);

    printf("Distance %f - station %d to station %d\n", psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters, g_sLocalStationInfo.m_un32StationId, psCam->header.stationID);
    //printf("Bearing %f - station %d to station %d\n", psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees, g_sLocalStationInfo.m_un32StationId, psCam->header.stationID);

    // --------------------------------------------------
    // ------------- Make Event Decision ----------------
    // --------------------------------------------------

    g_fp_its_processor_proccess_cam(psCam, psLocalFusionData, psRemoteFusionData);
}

static void sa_mngr_handle_denm(DENM *psDenm) {

    //printf("Handling received DENM message from %d\n", psDenm->header.stationID);

    SStationFullFusionData *psLocalFusionData = sa_database_get_station_data(g_sLocalStationInfo.m_un32StationId);

    if(NULL == psLocalFusionData) {

        printf("SA_MNGR - Failed to find local station id for computing haversine\n");
        return;
    }

    SStationFullFusionData *psRemoteFusionData = sa_database_get_station_data(psDenm->header.stationID);

    if(NULL == psRemoteFusionData) {

        psRemoteFusionData = sa_database_add_station_data(psDenm->header.stationID);

        if(NULL == psRemoteFusionData) {

            printf("SA_MNGR - Failed to find station %d fusion info\n", psDenm->header.stationID);
            return;
        }

        // Update additional station info.
        psRemoteFusionData->m_n32StationType = psDenm->denm.management.stationType;
    }

    //printf("SA_MNGR - Found station data of station %d\n", psDenm->header.stationID);

    // --------------------------------------------------
    // ------------- Make Event Decision ----------------
    // --------------------------------------------------

    g_fp_its_processor_proccess_denm(psDenm, psLocalFusionData, psRemoteFusionData);
}

static void sa_mngr_handle_publish(fix_data_t *psPotiFixData) {

    SStationFullFusionData *psStationData = sa_database_get_station_data(g_sLocalStationInfo.m_un32StationId);

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
    psStationData->m_sDistanceData.m_dLongitude = psPotiFixData->longitude;
    psStationData->m_sDistanceData.m_dLatitude = psPotiFixData->latitude;

    // Save timestamp.
    psStationData->m_dLastPotiTAI = psPotiFixData->time.tai_since_2004;

    //printf("Current time - %f\n", psPotiFixData->time.tai_since_2004);

    // ------------------------------------------------
    // ---------- Get New Data From Processor ---------
    // ------------------------------------------------

    g_fp_its_processor_process_tx(psPotiFixData);
}
