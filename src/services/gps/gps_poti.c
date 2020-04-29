#include "gps_poti.h"

#include "lib/inc/error_code_user.h"
#include "lib/inc/asn1defs_if.h"

#include "services/gps/gps_sim.h"
#include "services/gps/nmea_infra.h"

/*
 *******************************************************************************
 * Private variables
 *******************************************************************************
 */

poti_service_t *m_pPotiHandler;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t gps_poti_init() {

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    // Initialize POTI service.
    n32ProcedureResult = IS_SUCCESS(poti_create_service(&m_pPotiHandler, NULL));

    if(FALSE == IS_SUCCESS(n32ProcedureResult)) {

        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    // Initialize GPS simulator.
#ifdef __SIMULATOR_ENABLED__

    if(true == g_sScenarioInfo.m_bIsScenarioEnabled) {

        n32ProcedureResult = gps_sim_init(g_sScenarioInfo.m_achName, g_sScenarioInfo.m_achParticipantId);

        if(PROCEDURE_SUCCESSFULL != n32ProcedureResult) {

            printf("Cannot initialize gps simulator\n");
            return PROCEDURE_INVALID_SERVICE_INIT_ERROR;

        } else {

            gps_sim_pause_fix_data(true);

            if(0 == g_sScenarioInfo.m_un32GpSimSyncId
                || g_sScenarioInfo.m_un32GpSimSyncId == g_sLocalStationInfo.m_un32StationId) {

                gps_sim_pause_fix_data(false);
            }
        }
    }

#endif

    return n32ProcedureResult;

}

int32_t gps_poti_get_fix_data(fix_data_t *psPotiFixData) {

    if(NULL == psPotiFixData) {

        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    /* Wait for new GNSS update (10 Hz by default). */
    n32ProcedureResult = poti_wait_gnss_data(m_pPotiHandler, NULL);

    if(false == IS_SUCCESS(n32ProcedureResult)) {

        printf("poti_wait_gnss_data error: %s\n", ERROR_MSG(n32ProcedureResult));
        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    // Get fix data.
    n32ProcedureResult = poti_get_fix_data(m_pPotiHandler, psPotiFixData);

    if(false == IS_SUCCESS(n32ProcedureResult)) {

        printf("Cannot get GNSS fix data: %s\n", ERROR_MSG(n32ProcedureResult));
        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

#ifdef __SIMULATOR_ENABLED__

    gps_sim_update_fix_data(psPotiFixData);

#endif

    return n32ProcedureResult;
}

void gps_poti_sim_pause_fix_data(bool bIsPaused) {

    gps_sim_pause_fix_data(bIsPaused);
}

bool gps_poti_sim_is_paused() {

    return gps_sim_is_paused();
}

void gps_poti_release() {

#ifdef __SIMULATOR_ENABLED__

    gps_sim_release();

#endif

    poti_release_service(m_pPotiHandler);
}

void gps_poti_mngr_printf_poti(fix_data_t *psPotiFixData) {

    int32_t n32SentenceSize = 0;
    char achSentence[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    n32SentenceSize += snprintf(
                achSentence + n32SentenceSize,
                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                "T%d",
                g_sLocalStationInfo.m_un32StationId);

    // Convert POTI info to NMEA data.
    SNmeaGgaData sGgaData;
    SNmeaRmcData sRmcData;

    memset(&sGgaData, 0, sizeof(sGgaData));
    memset(&sRmcData, 0, sizeof(sRmcData));

    // Get data from POTI.
    nmea_get_gga_data(psPotiFixData, &sGgaData);
    nmea_get_rmc_data(psPotiFixData, &sRmcData);

    // Build and send NMEA sentence.
    n32SentenceSize += nmea_build_rmc_msg(&sRmcData, achSentence + n32SentenceSize);
    g_fp_write_to_boundary(achSentence, n32SentenceSize, g_sLocalStationInfo.m_un32StationId);
}
