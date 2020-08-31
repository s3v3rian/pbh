#include "its_msg_processor_passenger.h"

#include "lib/inc/gn_public.h"

#include "boundary/boundary_writer.h"

#include "common/containers/ring_buffer.h"
#include "common/containers/array_queue.h"

#include "common/utils/geo_utils.h"

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
static bool m_bIsAboutToCrossTL;
static bool m_bIsH2CReported;
static bool m_bIsH2OReported;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

bool its_msg_processor_passenger_init() {

    printf("Initializing Passenger ITS msg processor\n");

    m_bIsDoingStatusUpdate = false;
    m_bIsAboutToCrossTL = false;

    g_fp_h2_write_to_boundary_init("/dev/ttyAMA2");

    return its_msg_processor_init();
}

bool its_msg_processor_passenger_process_tx(fix_data_t *psPotiFixData) {

    // Prepare a CAM.
    CAM *psCam = NULL;
    if(false == its_msg_processor_allocate_tx_cam_msg(&psCam)) {

        printf("Failed to allocate from ring buffer, cam status update failed\n");
        return PROCEDURE_BUFFER_ERROR;
    }

    //if(true == m_bIsAboutToCrossTL) {

        //psCam->cam.camParameters.specialVehicleContainer_option = TRUE;
   // }
    psCam->header.stationID = 0;
    psCam->cam.camParameters.basicContainer.stationType = 0;
    psCam->cam.camParameters.basicContainer.referencePosition.latitude = 0;
    psCam->cam.camParameters.basicContainer.referencePosition.longitude = 0;

    // Send the CAM.
    return its_msg_processor_push_tx_cam_msg(psCam);
}

bool its_msg_processor_passenger_process_rx_cam(CAM *psCam, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData) {

    if(g_sLocalStationInfo.m_sVehicleInfo.m_dCollisionWarningThresholdInMeters > psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters
            && psRemoteFusionData->m_n32StationType != GN_ITS_STATION_ROAD_SIDE_UNIT) {

        if(StationaryVehicleSubCauseCode_vehicleBreakdown == psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode) {

            if(0.0 <= psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees
                    && 90.0 >= psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees) {

                g_fp_write_to_boundary_event(CauseCodeType_stationaryVehicle);
            }

            // TODO Previous code.
            //if(90.0 < geodesic_calculate_bearing(psRemoteFusionData->m_sDistanceData.m_dLatitude, psRemoteFusionData->m_sDistanceData.m_dLongitude, psLocalFusionData->m_sDistanceData.m_dLatitude, psLocalFusionData->m_sDistanceData.m_dLongitude)) {

              //  g_fp_write_to_boundary_event(CauseCodeType_stationaryVehicle);
            //}

        } else {

            // Tell host shit is going down!
            g_fp_write_to_boundary_event(CauseCodeType_collisionRisk);
        }
        /*
            DENM *psDenm = NULL;
            if(PROCEDURE_SUCCESSFULL != its_msg_processor_allocate_tx_denm_msg(&psDenm)) {

                printf("Failed to allocate from ring buffer, denm status update failed\n");
                return PROCEDURE_BUFFER_ERROR;
            }

            psDenm->denm.situation_option = TRUE;
            psDenm->denm.situation.eventType.causeCode = CauseCodeType_collisionRisk;

            its_msg_processor_push_tx_denm_msg(psDenm);
            */
    } else if(psRemoteFusionData->m_n32StationType == GN_ITS_STATION_ROAD_SIDE_UNIT) {

        if(TRUE == psCam->cam.camParameters.specialVehicleContainer_option) {

            printf("CauseCode is %d", psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode);
            if(TrafficConditionSubCauseCode_trafficRedLight == psCam->cam.camParameters.specialVehicleContainer.u.emergencyContainer.incidentIndication.subCauseCode) {

                g_fp_write_to_boundary_remote_event(CauseCodeType_trafficCondition, psRemoteFusionData->m_un32StationId);

                if(g_sLocalStationInfo.m_sVehicleInfo.m_dCollisionWarningThresholdInMeters > psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters
                        && 270.0 <= psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees
                        && 360.0 >= psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees) {

                    g_fp_write_to_boundary_event(CauseCodeType_dangerousSituation);
                }

                if(g_sLocalStationInfo.m_sVehicleInfo.m_dCollisionWarningThresholdInMeters > psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters
                        && 180.0 <= psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees
                        && 360.0 >= psRemoteFusionData->m_sDistanceData.m_dBearingToLocalInDegrees) {

                    g_fp_write_to_boundary_event(CauseCodeType_signalViolation);

                    if(false == m_bIsH2OReported) {

                        g_fp_h2_write_to_boundary_sentence("O", 1);
                        m_bIsH2CReported = false;
                        m_bIsH2OReported = true;
                    }

                } else {

                    if(false == m_bIsH2CReported) {

                        g_fp_h2_write_to_boundary_sentence("C", 1);
                        m_bIsH2CReported = true;
                        m_bIsH2OReported = false;
                    }
                }

            } else {

                if(false == m_bIsH2CReported) {

                    g_fp_h2_write_to_boundary_sentence("C", 1);
                    m_bIsH2CReported = true;
                    m_bIsH2OReported = false;
                }

                // If we recceived from an RSU and it is human problem then it is really a green light.
                g_fp_write_to_boundary_remote_event(CauseCodeType_humanProblem, psRemoteFusionData->m_un32StationId);
            }
        }

        //m_bIsAboutToCrossTL = true;
        //g_sLocalStationInfo.m_sVehicleInfo.m_dCollisionWarningThresholdInMeters > psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters

    } else {

       // m_bIsAboutToCrossTL = false;
    //    g_fp_write_to_boundary_event(0); TODO
    }

    return true;
}

bool its_msg_processor_passenger_process_rx_denm(DENM *psDenm, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData) {

    if(g_sLocalStationInfo.m_sVehicleInfo.m_dCollisionWarningThresholdInMeters > psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters) {

        if(CauseCodeType_trafficCondition == psDenm->denm.situation.eventType.causeCode
                && TrafficConditionSubCauseCode_trafficRedLight == psDenm->denm.situation.eventType.subCauseCode) {

            g_fp_write_to_boundary_event(CauseCodeType_signalViolation);
        }
    } else {

        g_fp_write_to_boundary_event(0);
    }

    return true;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
