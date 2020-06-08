#include "its_msg_processor_valeran_rsu.h"

#include <unistd.h>
#include <stdio.h>

#include "lib/inc/gn_public.h"

#include "common/globals.h"
#include "common/containers/ring_buffer.h"
#include "common/containers/array_queue.h"
#include "common/file/csv_infra.h"

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
static bool m_bIsSentShoulderVehicleInfo = false;

static double *m_pdLatitudeArray = NULL;
static double *m_pdLongitudeArray = NULL;

static int32_t m_n32LatitudeArrayIndex = 0;
static int32_t m_n32LongitudeArrayIndex = 0;
static int32_t m_n32LatitudeArraySize = 0;
static int32_t m_n32LongitudeArraySize = 0;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

bool its_msg_processor_valeran_rsu_init() {

    printf("Initializing Valeran RSU ITS msg processor\n");

    char achFilePath[1000];

    sprintf(achFilePath, "%s/simulator/stationary_vehicle/shoulder_vehicle/latitude.csv", g_pchConfigurationFileDirectoryPath);
    m_n32LatitudeArraySize = read_csv_doubles(achFilePath, &m_pdLatitudeArray);

    sprintf(achFilePath, "%s/simulator/stationary_vehicle/shoulder_vehicle/longitude.csv", g_pchConfigurationFileDirectoryPath);
    m_n32LongitudeArraySize = read_csv_doubles(achFilePath, &m_pdLongitudeArray);

    m_bIsDoingStatusUpdate = false;

    return its_msg_processor_init();
}

bool its_msg_processor_valeran_rsu_process_tx(fix_data_t *psPotiFixData) {

    // Call base function.
//    its_msg_processor_process_tx_pending_denms(psPotiFixData);

    if(true == m_bIsSentShoulderVehicleInfo) {

        // Prepare CAM message.
        CAM *psCam = NULL;
        if(false == its_msg_processor_allocate_tx_cam_msg(&psCam)) {

            printf("Failed to allocate from tx ring buffer, cam status update failed\n");
            return PROCEDURE_BUFFER_ERROR;
        }

        psCam->header.stationID = 0;
        psCam->cam.camParameters.basicContainer.stationType = 0;
        psCam->cam.camParameters.basicContainer.referencePosition.latitude = 0;
        psCam->cam.camParameters.basicContainer.referencePosition.longitude = 0;
        psCam->cam.camParameters.highFrequencyContainer.choice = HighFrequencyContainer_rsuContainerHighFrequency;
        psCam->cam.camParameters.highFrequencyContainer.u.rsuContainerHighFrequency.protectedCommunicationZonesRSU_option = FALSE;
        psCam->cam.camParameters.lowFrequencyContainer_option = FALSE;
        psCam->cam.camParameters.specialVehicleContainer_option = FALSE;

        // Set traffic light event data.
        //psDenm->denm.situation.eventType.causeCode = CauseCodeType_trafficCondition;
        //psDenm->denm.situation.eventType.subCauseCode = TrafficConditionSubCauseCode_trafficGreenLight;
        psCam->cam.camParameters.specialVehicleContainer_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_emergencyContainer;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.causeCode = CauseCodeType_trafficCondition;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode = TrafficConditionSubCauseCode_trafficGreenLight;

        // Always send a CAM.
        its_msg_processor_push_tx_cam_msg(psCam);
        m_bIsSentShoulderVehicleInfo = false;

    } else {

        CAM *psCam2 = NULL;
        if(false == its_msg_processor_allocate_tx_cam_msg(&psCam2)) {

            printf("Failed to allocate from tx ring buffer, cam status update failed\n");
            return PROCEDURE_BUFFER_ERROR;
        }

        // Set CAM container options.
        psCam2->header.stationID = 444;
        psCam2->cam.camParameters.basicContainer.stationType = GN_ITS_STATION_PASSENGER_CAR;
        psCam2->cam.camParameters.basicContainer.referencePosition.latitude = (int32_t)(m_pdLatitudeArray[m_n32LatitudeArrayIndex] * 10000000.0);
        psCam2->cam.camParameters.basicContainer.referencePosition.longitude = (int32_t)(m_pdLongitudeArray[m_n32LongitudeArrayIndex] * 10000000.0);
        psCam2->cam.camParameters.highFrequencyContainer.choice = HighFrequencyContainer_rsuContainerHighFrequency;
        psCam2->cam.camParameters.highFrequencyContainer.u.rsuContainerHighFrequency.protectedCommunicationZonesRSU_option = FALSE;
        psCam2->cam.camParameters.lowFrequencyContainer_option = FALSE;
        psCam2->cam.camParameters.specialVehicleContainer_option = TRUE;
        psCam2->cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_emergencyContainer;
        psCam2->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication_option = TRUE;
        psCam2->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.causeCode = CauseCodeType_stationaryVehicle;
        psCam2->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode = StationaryVehicleSubCauseCode_vehicleBreakdown;

        g_fp_write_to_boundary_remote_event(CauseCodeType_stationaryVehicle, 444);

        its_msg_processor_push_tx_cam_msg(psCam2);
        m_bIsSentShoulderVehicleInfo = true;
    }
    /*
    // Prepare general status message.
    DENM *psDenm = NULL;
    if(false == its_msg_processor_allocate_tx_denm_msg(&psDenm)) {

        printf("Failed to allocate from tx ring buffer, denm status update failed\n");
        return PROCEDURE_BUFFER_ERROR;
    }

    psDenm->denm.situation_option = TRUE;

    psDenm->denm.situation.informationQuality = 0;
    psDenm->denm.situation.linkedCause_option = FALSE;
    psDenm->denm.situation.eventHistory_option = FALSE;

    // If currently red light is on then send traffic condition message.
    if(true == g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight) {

        // Set traffic light event data.
        psDenm->denm.situation.eventType.causeCode = CauseCodeType_trafficCondition;
        psDenm->denm.situation.eventType.subCauseCode = TrafficConditionSubCauseCode_trafficRedLight;
        psCam->cam.camParameters.specialVehicleContainer_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_emergencyContainer;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.causeCode = CauseCodeType_trafficCondition;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode = TrafficConditionSubCauseCode_trafficRedLight;

    } else {

        // Set traffic light event data.
        psDenm->denm.situation.eventType.causeCode = CauseCodeType_trafficCondition;
        psDenm->denm.situation.eventType.subCauseCode = TrafficConditionSubCauseCode_trafficGreenLight;
        psCam->cam.camParameters.specialVehicleContainer_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_emergencyContainer;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication_option = TRUE;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.causeCode = CauseCodeType_trafficCondition;
        psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode = TrafficConditionSubCauseCode_trafficGreenLight;
    }

*/

    //printf("Pushing DENM periodic traffic light status\n");

    // Send periodic status.
    //its_msg_processor_push_tx_denm_msg(psDenm);

    // TODO Get procedure reuslt from above procedures.
    return true;
}

bool its_msg_processor_valeran_rsu_process_rx_cam(CAM *psCam, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData) {

    /*
    if(true == g_sLocalStationInfo.m_sRsuInfo.m_usSpecifics.m_sTrafficLightInfo.m_bIsRedLight) {

        g_fp_write_to_boundary_event(CauseCodeType_trafficCondition);

    } else {

        g_fp_write_to_boundary_event(CauseCodeType_humanProblem);
    }
    */
    /*
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
    */

    return true;
}

bool its_msg_processor_valeran_rsu_process_rx_denm(DENM *psDenm, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData) {

    return true;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
