#include "its_msg_processor_rsu.h"

#include <unistd.h>

#include "lib/inc/gn_public.h"

#include "common/containers/ring_buffer.h"
#include "common/containers/array_queue.h"

#include "sa/processors/its_msg_processor.h"

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

static bool m_bIsDoingStatusUpdate;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t its_msg_processor_rsu_init() {

    printf("Initializing RSU ITS msg processor\n");

    int32_t n32ProcedureResult = its_msg_processor_init();

    m_bIsDoingStatusUpdate = false;

    return n32ProcedureResult;
}

int32_t its_msg_processor_rsu_process_tx(fix_data_t *psPotiFixData) {

    // Call base function.
//    its_msg_processor_process_tx_pending_denms(psPotiFixData);

    // Prepare CAM message.
    CAM *psCam = NULL;
    if(PROCEDURE_SUCCESSFULL != its_msg_processor_allocate_tx_cam_msg(&psCam)) {

        printf("Failed to allocate from ring buffer, cam status update failed\n");
        return PROCEDURE_BUFFER_ERROR;
    }

    // Set CAM container options.
    psCam->cam.camParameters.highFrequencyContainer.choice = HighFrequencyContainer_rsuContainerHighFrequency;
    psCam->cam.camParameters.highFrequencyContainer.u.rsuContainerHighFrequency.protectedCommunicationZonesRSU_option = FALSE;
    psCam->cam.camParameters.lowFrequencyContainer_option = FALSE;
    psCam->cam.camParameters.specialVehicleContainer_option = FALSE;

    // Always send a CAM.
    its_msg_processor_push_tx_cam_msg(psCam);
    its_msg_processor_process_tx_cam(psPotiFixData);

    // Prepare general status message.
    DENM *psDenm = NULL;
    if(PROCEDURE_SUCCESSFULL != its_msg_processor_allocate_tx_denm_msg(&psDenm)) {

        printf("Failed to allocate from ring buffer, denm status update failed\n");
        return PROCEDURE_BUFFER_ERROR;
    }

    psDenm->denm.situation_option = TRUE;

    // If currently red light is on then send traffic condition message.
    if(true == g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight) {

        // Set traffic light event data.
        psDenm->denm.situation.eventType.causeCode = CauseCodeType_trafficCondition;
        psDenm->denm.situation.eventType.subCauseCode = TrafficConditionSubCauseCode_trafficRedLight;

    } else {

        // Set traffic light event data.
        psDenm->denm.situation.eventType.causeCode = CauseCodeType_trafficCondition;
        psDenm->denm.situation.eventType.subCauseCode = TrafficConditionSubCauseCode_trafficGreenLight;
    }

    // Set LLA data.
    psDenm->denm.management.eventPosition.latitude = psPotiFixData->latitude * 10000000.0;
    psDenm->denm.management.eventPosition.longitude = psPotiFixData->longitude * 10000000.0;
    psDenm->denm.management.transmissionInterval_option = FALSE;
    psDenm->denm.management.transmissionInterval = 1;

    //printf("Pushing DENM periodic traffic light status\n");

    // Send periodic status.
    its_msg_processor_push_tx_denm_msg(psDenm);
    its_msg_processor_process_tx_denm(psPotiFixData);

    // TODO Get procedure reuslt from above procedures.
    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_process_rx_cam(CAM *psCam, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData) {

    // If currently red light is on and current distance to target is less then X meters then send signal violation.
    if(true == g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight) {

        if(g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_n32SignalViolationThresholdInMeters > psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters) {

            DENM *psDenm = NULL;
            if(PROCEDURE_SUCCESSFULL != its_msg_processor_allocate_tx_denm_msg(&psDenm)) {

                printf("Failed to allocate from ring buffer, denm status update failed\n");
                return PROCEDURE_BUFFER_ERROR;
            }

            psDenm->denm.situation_option = TRUE;
            psDenm->denm.situation.eventType.causeCode = CauseCodeType_signalViolation;

            //its_msg_processor_push_tx_denm_msg(psDenm);
        }
    }

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_process_rx_denm(DENM *psDenm, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData) {

    return PROCEDURE_SUCCESSFULL;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
