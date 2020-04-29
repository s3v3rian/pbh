#include "its_msg_processor_commercial.h"

#include "lib/inc/gn_public.h"

#include "common/containers/ring_buffer.h"
#include "common/containers/array_queue.h"

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

static int32_t m_n32CamQueueId	= INVALID_CONTAINER_ID;
static int32_t m_n32DenmQueueId = INVALID_CONTAINER_ID;

static int32_t m_n32CamTxRingBufferId = INVALID_CONTAINER_ID;
static int32_t m_n32DenmTxRingBufferId = INVALID_CONTAINER_ID;

static bool m_bIsDoingDenmStatusUpdate;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t its_msg_processor_cm_init() {

    m_n32CamQueueId = array_queue_container_init("sa_its_processor_cam_queue");
    m_n32DenmQueueId = array_queue_container_init("sa_its_processor_denm_queue");

    m_n32CamTxRingBufferId = ring_buffer_container_init("cam_tx_ring_buffer", sizeof(CAM));
    m_n32DenmTxRingBufferId = ring_buffer_container_init("denm_tx_ring_buffer", sizeof(DENM));

    if(0 > m_n32CamQueueId
            || 0 > m_n32DenmQueueId
            || 0 > m_n32CamTxRingBufferId
            || 0 > m_n32DenmTxRingBufferId) {

        printf("Cannot init sa containers\n");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    // On first receiving from RSU do a status update.
    m_bIsDoingDenmStatusUpdate = true;

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_cm_process_cam(CAM *psCam, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData) {

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    if(GN_ITS_STATION_ROAD_SIDE_UNIT == psRemoteStationData->m_n32StationType) {

        /*
         * If remote is a rsu then make decision on how to handle next denm status.
         */
        if(false == m_bIsDoingDenmStatusUpdate) {

            CAM *psCam = NULL;
            DENM *psDenm = NULL;

            if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32CamTxRingBufferId, (char**)(&psCam))) {

                printf("Failed to allocate from ring buffer, denm status update failed\n");
                n32ProcedureResult = PROCEDURE_BUFFER_ERROR;
            }

            if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32DenmTxRingBufferId, (char**)(&psDenm))) {

                printf("Failed to allocate from ring buffer, denm status update failed\n");
                n32ProcedureResult = PROCEDURE_BUFFER_ERROR;
            }

            // Set vehicle dimensions.
            psCam->cam.camParameters.highFrequencyContainer.choice = HighFrequencyContainer_basicVehicleContainerHighFrequency;
            psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleWidth = g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_n32VehicleWidth;
            psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue = g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_n32VehicleLength;
            psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthConfidenceIndication = VehicleLengthConfidenceIndication_noTrailerPresent;

            // TODO Remove this.
            psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.heading.headingValue = g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_n32VehicleWeight;
            psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.heading.headingConfidence = g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_n32VehicleHeight;

            if(TRUE == g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_bIsTrailerAttached) {

                // Set length with trailer.
                if(0 < g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_n32TrailerLength) {

                    psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue = g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_n32VehicleLength + g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_n32TrailerLength;
                    psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthConfidenceIndication = VehicleLengthConfidenceIndication_trailerPresentWithKnownLength;

                } else {

                    psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthConfidenceIndication = VehicleLengthConfidenceIndication_trailerPresentWithUnknownLength;
                }

                // Set widht with trailer.
                if(0 < g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_n32TrailerWidth) {

                    psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue = g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_n32VehicleWidth + g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_n32TrailerWidth;
                }
            }

            // Set vehicle dangerous goods info.
            if(TRUE == g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_bIsDangerousGoods) {

                psCam->cam.camParameters.specialVehicleContainer_option = TRUE;
                psCam->cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_dangerousGoodsContainer;
                psCam->cam.camParameters.specialVehicleContainer.u.dangerousGoodsContainer.dangerousGoodsBasic = (DangerousGoodsBasic)g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_eDangerousGoodsType;
                psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods_option = TRUE;
                psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.dangerousGoodsType = (DangerousGoodsBasic)g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_eDangerousGoodsType;
            }

            // Set DENM event.
            psDenm->denm.situation_option = TRUE;
            psDenm->denm.situation.eventType.causeCode = CauseCodeType_commercialVehicleSituation;

            // Set vehicle id.
            psDenm->denm.alacarte_option = TRUE;
            psDenm->denm.alacarte.stationaryVehicle_option = TRUE;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification_option = TRUE;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber_option = TRUE;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber.buf = (uint8_t*)g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_achVehicleDriverName;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber.len = strlen(g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_achVehicleDriverName) + 1;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS_option = TRUE;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS.buf = (uint8_t*)g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_achVehicleLicensePlate;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS.len = strlen(g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_sVehicleInfo.m_achVehicleLicensePlate) + 1;

            // Set denm event locations.
            psDenm->denm.management.eventPosition.latitude = (int32_t)(g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_dDeparturePointLatitude * 10000000.0); /* Convert to 1/10 micro degree. */
            psDenm->denm.management.eventPosition.longitude = (int32_t)(g_sLocalStationInfo.m_usAdditionals.m_sCommercialStationInfo.m_dDeparturePointLongitude * 10000000.0); /* Convert to 1/10 micro degree. */

            array_queue_container_push(m_n32CamQueueId, 0, (char*)psCam);
            array_queue_container_push(m_n32DenmQueueId, 0, (char*)psDenm);
        }
    }

    return n32ProcedureResult;
}

int32_t its_msg_processor_cm_process_denm(DENM *psDenm, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData) {

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_cm_process_poti_cam(fix_data_t *psPotiFixData, CAM **p2sCam) {

    int32_t n32ElementId = 0;
    char *pchData = NULL;

    array_queue_container_pop(m_n32CamQueueId, &n32ElementId, &pchData);

    if(NULL == pchData) {

        printf("Allocating default cam element\n");

        if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32CamTxRingBufferId, &pchData)) {

            return PROCEDURE_BUFFER_ERROR;
        }
    }

    *p2sCam = (CAM*)(pchData);

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_cm_process_poti_denm(fix_data_t *psPotiFixData, DENM **p2sDenm) {

    int32_t n32ElementId = 0;
    char *pchData = NULL;

    array_queue_container_pop(m_n32DenmQueueId, &n32ElementId, &pchData);

    if(NULL == pchData) {

        printf("Allocating default denm element\n");

        if(PROCEDURE_SUCCESSFULL != ring_buffer_container_allocate(m_n32DenmTxRingBufferId, &pchData)) {

            return PROCEDURE_BUFFER_ERROR;
        }
    }

    *p2sDenm = (DENM*)(pchData);

    /* Geographical position of the detected event. */
//    (*p2sDenm)->denm.management.eventPosition.positionConfidenceEllipse.semiMajorConfidence = DENM_EVENT_GET_POSITION_MAJOR_CONFIDENCE(); /* Convert to centimetre. */
//    (*p2sDenm)->denm.management.eventPosition.positionConfidenceEllipse.semiMinorConfidence = DENM_EVENT_GET_POSITION_MINOR_CONFIDENCE(); /* Convert to centimetre. */
//    (*p2sDenm)->denm.management.eventPosition.positionConfidenceEllipse.semiMajorOrientation = (int32_t)(DENM_EVENT_GET_POSITION_MAJOR_ORIENTATION() * 10.0); /* Convert to 0.1 degree from North. */
//    (*p2sDenm)->denm.management.eventPosition.altitude.altitudeValue = (int32_t)(psPotiFixData->altitude * 100.0); /* Convert to 0.01 metre. */
//    (*p2sDenm)->denm.management.eventPosition.altitude.altitudeConfidence = DENM_EVENT_GET_POSITION_ALT_CONFIDENCE();

    // Reset status update.
    m_bIsDoingDenmStatusUpdate = true;

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_cm_clear_cam() {

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_cm_clear_denm() {

    return PROCEDURE_SUCCESSFULL;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
