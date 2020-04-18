#include "sa_mngr.h"

#include "lib/inc/error_code_user.h"
#include "common/utils/print_utils.h"
#include "gps/nmea_infra.h"
#include "gps/gps_sim.h"

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

void sa_mngr_init() {

    cam_mngr_init();
    denm_mngr_init();
}

void sa_mngr_process_tx() {

    fix_data_t sPotiFixData = FIX_DATA_INIT;
    int32_t n32GnssUpdateCount = 0;
    int32_t n32ProcedureResult = 0;

    /* Wait for new GNSS update (10 Hz by default). */
    n32ProcedureResult = poti_wait_gnss_data(g_psPotiHandler, NULL);

    n32GnssUpdateCount++;

    // Send every 1 second.
    if(g_sTxParameters.m_n32TxFrequencyIn10Hz > n32GnssUpdateCount) {

        return;
    }

    n32GnssUpdateCount = 0;

    if(false == IS_SUCCESS(n32ProcedureResult)) {

        printf("poti_wait_gnss_data error: %s\n", ERROR_MSG(n32ProcedureResult));
        return;
    }

    /* Get fix data. */
    n32ProcedureResult = poti_get_fix_data(g_psPotiHandler, &sPotiFixData);

    if(false == IS_SUCCESS(n32ProcedureResult)) {

        printf("Cannot get GNSS fix data: %s\n", ERROR_MSG(n32ProcedureResult));
        return;
    }

    /* Make sure the navigation information is valid. */
    if(sPotiFixData.status == FIX_STATUS_NA
                || sPotiFixData.mode == FIX_MODE_NA
                || sPotiFixData.mode == FIX_MODE_NO_FIX) {

        printf("Navigation information is invalid\n");
    }

    gps_sim_update_fix_data(&sPotiFixData);;
    cam_mngr_process_tx(&sPotiFixData);
    denm_mngr_process_tx(&sPotiFixData);

    sa_mngr_output_poti(&sPotiFixData);

    fflush(stdout);
}

void sa_mngr_process_rx_cam() {

    CAM *psCam = NULL;

    if(PROCEDURE_SUCCESSFULL != cam_mngr_process_rx(&psCam)) {

        return;
    }

    sa_mngr_output_cam(psCam);

#ifdef __GPS_SIMULATOR_ENABLED__

    if(true == g_sScenarioInfo.m_bIsGpsSimEnabled) {

        if(true == gps_sim_is_paused()) {

            if(psCam->header.stationID == g_sScenarioInfo.m_un32GpSimSyncId) {

                gps_sim_pause_fix_data(false);
            }
        }
    }
#endif
}

void sa_mngr_process_rx_denm() {

    DENM *psDenm = NULL;

    if(PROCEDURE_SUCCESSFULL != denm_mngr_process_rx(&psDenm)) {

        return;
    }

    sa_mngr_output_denm(psDenm);
}

void sa_mngr_release() {

    cam_mngr_release();
    denm_mngr_release();
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
