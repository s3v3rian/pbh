#include "its_msg_processor_passenger.h"

#include "lib/inc/gn_public.h"

#include "boundary/boundary_writer.h"

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

bool its_msg_processor_passenger_init() {

    printf("Initializing Passenger ITS msg processor\n");

    m_bIsDoingStatusUpdate = false;

    return its_msg_processor_init();
}

bool its_msg_processor_passenger_process_tx(fix_data_t *psPotiFixData) {

    // Prepare a CAM.
    CAM *psCam = NULL;
    if(false == its_msg_processor_allocate_tx_cam_msg(&psCam)) {

        printf("Failed to allocate from ring buffer, cam status update failed\n");
        return PROCEDURE_BUFFER_ERROR;
    }

    // Send the CAM.
    return its_msg_processor_push_tx_cam_msg(psCam);
}

bool its_msg_processor_passenger_process_rx_cam(CAM *psCam, SStationFullFusionData *psLocalFusionData, SStationFullFusionData *psRemoteFusionData) {

    if(g_sLocalStationInfo.m_sVehicleInfo.m_dCollisionWarningThresholdInMeters > psRemoteFusionData->m_sDistanceData.m_dDistanceToLocalInMeters
            && psRemoteFusionData->m_n32StationType != GN_ITS_STATION_ROAD_SIDE_UNIT) {

        // Tell host shit is going down!
        g_fp_write_to_boundary_event(CauseCodeType_collisionRisk);
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
    } else {

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
