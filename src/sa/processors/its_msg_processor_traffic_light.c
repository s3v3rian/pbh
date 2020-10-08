#include "its_msg_processor_traffic_light.h"

#include <unistd.h>

#include "lib/inc/gn_public.h"

#include "common/containers/ring_buffer.h"
#include "common/containers/array_queue.h"

#include "common/utils/geo_utils.h"

#include "boundary/boundary_writer.h"

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

bool its_msg_processor_traffic_light_init() {

    printf("Initializing RSU ITS msg processor\n");

    m_bIsDoingStatusUpdate = false;

    return its_msg_processor_init();
}

bool its_msg_processor_traffic_light_process_tx(fix_data_t *psPotiFixData) {

    // Call base function.
//    its_msg_processor_process_tx_pending_denms(psPotiFixData);

    // Prepare CAM message.
    CAM *psCam = NULL;
    if(false == its_msg_processor_allocate_tx_cam_msg(&psCam)) {

        printf("Failed to allocate from tx ring buffer, cam status update failed\n");
        return PROCEDURE_BUFFER_ERROR;
    }

    // Set CAM container options.
    psCam->header.stationID = 0;
    psCam->cam.camParameters.basicContainer.stationType = 0;
    psCam->cam.camParameters.basicContainer.referencePosition.latitude = 0;
    psCam->cam.camParameters.basicContainer.referencePosition.longitude = 0;
    psCam->cam.camParameters.highFrequencyContainer.choice = HighFrequencyContainer_rsuContainerHighFrequency;
    psCam->cam.camParameters.highFrequencyContainer.u.rsuContainerHighFrequency.protectedCommunicationZonesRSU_option = FALSE;
    psCam->cam.camParameters.lowFrequencyContainer_option = FALSE;
    psCam->cam.camParameters.specialVehicleContainer_option = FALSE;

    // Prepare general status message.
    /*
    DENM *psDenm = NULL;
    if(false == its_msg_processor_allocate_tx_denm_msg(&psDenm)) {

        printf("Failed to allocate from tx ring buffer, denm status update failed\n");
        return PROCEDURE_BUFFER_ERROR;
    }
    */

    //psDenm->denm.situation_option = TRUE;

    //psDenm->denm.situation.informationQuality = 0;
    //psDenm->denm.situation.linkedCause_option = FALSE;
    //psDenm->denm.situation.eventHistory_option = FALSE;

    // If currently red light is on then send traffic condition message.
    if(true == g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight) {

        // Set traffic light event data.
        //psDenm->denm.situation.eventType.causeCode = CauseCodeType_trafficCondition;
        //psDenm->denm.situation.eventType.subCauseCode = TrafficConditionSubCauseCode_trafficRedLight;
        psCam->cam.camParameters.specialVehicleContainer_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_emergencyContainer;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.causeCode = CauseCodeType_trafficCondition;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode = TrafficConditionSubCauseCode_trafficRedLight;

    } else {

        // Set traffic light event data.
        //psDenm->denm.situation.eventType.causeCode = CauseCodeType_trafficCondition;
        //psDenm->denm.situation.eventType.subCauseCode = TrafficConditionSubCauseCode_trafficGreenLight;
        psCam->cam.camParameters.specialVehicleContainer_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_emergencyContainer;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.causeCode = CauseCodeType_trafficCondition;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode = TrafficConditionSubCauseCode_trafficGreenLight;
    }

    // Always send a CAM.
    its_msg_processor_push_tx_cam_msg(psCam);

    //printf("Pushing DENM periodic traffic light status\n");

    // Send periodic status.
    //its_msg_processor_push_tx_denm_msg(psDenm);

    // TODO Get procedure reuslt from above procedures.
    return true;
}

bool its_msg_processor_traffic_light_process_rx_cam(CAM *psCam, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData) {

    if(true == g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight) {

        if(g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_n32SignalViolationThresholdInMeters > psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters) {

            if(GN_ITS_STATION_BUS == psRemoteFusionData->m_n32StationType
                    && 90.0 < psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees
                    && 180.0 > psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees) {

                g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight = false;
                g_fp_write_to_boundary_event(CauseCodeType_humanProblem);

            } else if(GN_ITS_STATION_BUS != psRemoteFusionData->m_n32StationType
                      && g_sLocalStationInfo.m_sRsuInfo.m_dMaxSignalViolationBearing >= psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees) {

                g_fp_write_to_boundary_event(CauseCodeType_signalViolation);

            } else {

                if(GN_ITS_STATION_BUS != psRemoteFusionData->m_n32StationType
                      && g_sLocalStationInfo.m_sRsuInfo.m_dMinDangerousSituationBearing <= psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees) {

                    g_fp_write_to_boundary_remote_event(CauseCodeType_dangerousSituation, psRemoteFusionData->m_un32StationId);
                }

                g_fp_write_to_boundary_event(CauseCodeType_trafficCondition);
            }

        } else {

            g_fp_write_to_boundary_event(CauseCodeType_trafficCondition);
        }

        if(GN_ITS_STATION_PASSENGER_CAR == psRemoteFusionData->m_n32StationType) {

            char achSentence[512];
            int32_t n32SentenceSize = sprintf(achSentence, "T%u,$VSU,%d%d\n",
                                      psRemoteFusionData->m_un32StationId,
                                      psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedValue,
                                      psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.heading.headingValue);
            g_fp_write_to_boundary_sentence(achSentence, n32SentenceSize);
        }

    } else {

        if(GN_ITS_STATION_PASSENGER_CAR == psRemoteFusionData->m_n32StationType) {

            char achSentence[512];
            int32_t n32SentenceSize = sprintf(achSentence, "T%u,$VSU,%d%d\n",
                                      psRemoteFusionData->m_un32StationId,
                                      psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedValue,
                                      psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.heading.headingValue);
            g_fp_write_to_boundary_sentence(achSentence, n32SentenceSize);
        }

        g_fp_write_to_boundary_event(CauseCodeType_humanProblem);
    }

    return true;
}

bool its_msg_processor_traffic_light_process_rx_denm(DENM *psDenm, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData) {

    return true;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
