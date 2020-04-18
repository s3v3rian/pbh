#include "sa_mngr.h"

#include "lib/inc/error_code_user.h"
#include "common/containers/blocked_array_queue.h"
#include "common/utils/print_utils.h"
#include "services/gps/gps_poti.h"
#include "services/gps/nmea_infra.h"

#include "sa/sa_database.h"

/*
 *******************************************************************************
 * Private/Extern variables
 *******************************************************************************
 */

int32_t m_n32FusionContainerId = INVALID_QUEUE_ID;

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

    m_n32FusionContainerId = blocked_array_queue_container_init("sa_fusion_queue");

    if(0 < m_n32FusionContainerId) {

        printf("Cannot init sa fusion container\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    return n32ProcedureResult;
}

void sa_mngr_process_fusion() {

    int32_t n32MsgId = 0;
    char *pchMsgData = NULL;

    blocked_array_queue_container_pop(m_n32FusionContainerId, &n32MsgId, &pchMsgData);

    // Handle spurious wakeups.
    if(NULL == pchMsgData
            || INVALID_QUEUE_ELEMENT_ID == n32MsgId) {

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

    fix_data_t sPotiFixData = FIX_DATA_INIT;
    int32_t n32GnssUpdateCount = 0;
    int32_t n32ProcedureResult = 0;

    n32ProcedureResult = gps_poti_get_fix_data(&sPotiFixData);
    n32GnssUpdateCount++;

    if(g_sTxParameters.m_n32TxFrequencyIn10Hz > n32GnssUpdateCount) {

        return;
    }

    n32GnssUpdateCount = 0;

    /* Make sure the navigation information is valid. */
    if(sPotiFixData.status == FIX_STATUS_NA
                || sPotiFixData.mode == FIX_MODE_NA
                || sPotiFixData.mode == FIX_MODE_NO_FIX) {

        printf("Navigation information is invalid\n");
    }

    sa_mngr_output_poti(&sPotiFixData);

    blocked_array_queue_container_push(m_n32FusionContainerId, POTI_id, (char*)&sPotiFixData);

    fflush(stdout);
}

void sa_mngr_process_cam() {

    CAM *psOutputCam = cam_mngr_allocate_encoded_buffer();

    if(NULL == psOutputCam) {

        printf("Failed to allocate rx cam resources\n");
        return;
    }

    if(PROCEDURE_SUCCESSFULL != cam_mngr_process_rx(psOutputCam)) {

        return;
    }

    sa_mngr_output_cam(psOutputCam);

    blocked_array_queue_container_push(m_n32FusionContainerId, CAM_Id, (char*)psOutputCam);

    /*
#ifdef __GPS_SIMULATOR_ENABLED__

    if(true == g_sScenarioInfo.m_bIsGpsSimEnabled) {

        if(true == gps_sim_is_paused()) {

            if(psOutputCam->header.stationID == g_sScenarioInfo.m_un32GpSimSyncId) {

                gps_sim_pause_fix_data(false);
            }
        }
    }
#endif
*/
}

void sa_mngr_process_denm() {

    DENM *psOutputDenm = denm_mngr_allocate_encoded_buffer();

    if(NULL == psOutputDenm) {

        printf("Failed to allocate rx denm resources\n");
        return;
    }


    if(PROCEDURE_SUCCESSFULL != denm_mngr_process_rx(psOutputDenm)) {

        return;
    }

    sa_mngr_output_denm(psOutputDenm);

    blocked_array_queue_container_push(m_n32FusionContainerId, DENM_Id, (char*)psOutputDenm);
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

            printf("Received unknown message data - %d\n", n32MsgId);
            break;
    }
}

static void sa_mngr_update_db_cam(CAM *psCam) {

    SStationFusionData *psStationData = sa_database_get_station_data(psCam->header.stationID);

    if(NULL == psStationData) {

        return;
    }

    printf("Found station data of station %d\n", psCam->header.stationID);
}

static void sa_mngr_update_db_denm(DENM *psDenm) {

    SStationFusionData *psStationData = sa_database_get_station_data(psDenm->header.stationID);

    if(NULL == psStationData) {

        return;
    }

    printf("Found station data of station %d\n", psDenm->header.stationID);
}

static void sa_mngr_publish(fix_data_t *psPotiFixData) {

    // ------------------------------------------------
    // --------------- Allocate Resources -------------
    // ------------------------------------------------

    CAM *psOutputCam = cam_mngr_allocate_encoded_buffer();
    DENM *psOutputDenm = denm_mngr_allocate_encoded_buffer();

    if(NULL == psOutputCam
            || NULL == psOutputDenm) {

        printf("Failed to allocate tx resources\n");
        return;
    }

    // ------------------------------------------------
    // ---------------- Publish Situation -------------
    // ------------------------------------------------

    cam_mngr_process_tx(psPotiFixData, psOutputCam);
    denm_mngr_process_tx(psPotiFixData, psOutputDenm);
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
    n32Result = g_fpBoundaryWriter(achNmeaSentence, n32SentenceSize, g_sStationInfo.m_un32StationId);

    // Build and send NMEA sentence.
    n32SentenceSize = nmea_build_rmc_msg(&sRmcData, achNmeaSentence);
    n32Result &= g_fpBoundaryWriter(achNmeaSentence, n32SentenceSize, g_sStationInfo.m_un32StationId);

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

    if(TRUE == psDenm->denm.situation_option) {

        char achSentence[300];
        int32_t n32SentenceSize = sprintf_denm_event_code(achSentence, psDenm->denm.situation.eventType.causeCode);
        n32Result = g_fpBoundaryWriter(achSentence, n32SentenceSize, psDenm->header.stationID);
    }

    return n32Result;
}
