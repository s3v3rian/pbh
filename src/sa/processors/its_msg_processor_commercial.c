#include "its_msg_processor_commercial.h"

#include "lib/inc/gn_public.h"

#include "common/containers/ring_buffer.h"
#include "common/containers/array_queue.h"

#include "sa/processors/its_msg_processor.h"

/*
 *******************************************************************************
 * Private function signatures
 *******************************************************************************
 */

int32_t parameters_info_ini_loader(void* pchUser, const char* pchSection, const char* pchName, const char* pchValue);

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

static bool m_bIsDoingGeneralStatusUpdate;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t its_msg_processor_cm_init() {

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    n32ProcedureResult = its_msg_processor_init();

    // On first receiving from RSU do a status update.
    m_bIsDoingGeneralStatusUpdate = false;

    return n32ProcedureResult;
}

int32_t its_msg_processor_cm_process_poti(fix_data_t *psPotiFixData) {

    m_bIsDoingGeneralStatusUpdate = false;

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_cm_process_rx_cam(CAM *psCam, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData) {

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    if(GN_ITS_STATION_ROAD_SIDE_UNIT == psRemoteStationData->m_n32StationType) {

        //printf("Preparing status update for road side unit\n");

        /*
         * If remote is a rsu then make decision on how to handle next denm status.
         */
        if(false == m_bIsDoingGeneralStatusUpdate) {

            CAM *psCam = NULL;
            DENM *psDenm = NULL;
            DENM *psDenm2 = NULL;

            if(PROCEDURE_SUCCESSFULL != its_msg_processor_get_tx_cam_msg(&psCam)) {

                printf("Failed to allocate from ring buffer, denm status update failed\n");
                n32ProcedureResult = PROCEDURE_BUFFER_ERROR;
            }

            if(PROCEDURE_SUCCESSFULL != its_msg_processor_get_tx_denm_msg(&psDenm)) {

                printf("Failed to allocate from ring buffer, denm status update failed\n");
                n32ProcedureResult = PROCEDURE_BUFFER_ERROR;
            }

            if(PROCEDURE_SUCCESSFULL != its_msg_processor_get_tx_denm_msg(&psDenm2)) {

                printf("Failed to allocate from ring buffer, denm status update failed\n");
                n32ProcedureResult = PROCEDURE_BUFFER_ERROR;
            }

            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_bIsDangerousGoods = TRUE;
            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_eDangerousGoodsType = DangerousGoodsBasic_explosives1;
            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDepartureLocation.m_dLatitude = 50.5115286;
            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDepartureLocation.m_dLongitude = 13.3576199;
            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDestinationLocation.m_dLatitude = 50.0596697;
            g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDestinationLocation.m_dLongitude = 14.4656239;

            // Set vehicle dimensions.
            psCam->cam.camParameters.highFrequencyContainer.choice = HighFrequencyContainer_basicVehicleContainerHighFrequency;
            psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleWidth = g_sLocalStationInfo.m_sVehicleInfo.m_n32VehicleWidth;
            psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue = g_sLocalStationInfo.m_sVehicleInfo.m_n32VehicleLength;
            psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthConfidenceIndication = VehicleLengthConfidenceIndication_noTrailerPresent;

            if(TRUE == g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_bIsTrailerAttached) {

                // Set length with trailer.
                if(0 < g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_n32TrailerLength) {

                    psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue = g_sLocalStationInfo.m_sVehicleInfo.m_n32VehicleLength + g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_n32TrailerLength;
                    psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthConfidenceIndication = VehicleLengthConfidenceIndication_trailerPresentWithKnownLength;

                } else {

                    psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthConfidenceIndication = VehicleLengthConfidenceIndication_trailerPresentWithUnknownLength;
                }

                // Set widht with trailer.
                if(0 < g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_n32TrailerWidth) {

                    psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue = g_sLocalStationInfo.m_sVehicleInfo.m_n32VehicleWidth + g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_n32TrailerWidth;
                }
            }

            // Set vehicle dangerous goods info.
            if(TRUE == g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_bIsDangerousGoods) {

                psCam->cam.camParameters.specialVehicleContainer_option = TRUE;
                psCam->cam.camParameters.specialVehicleContainer.choice = SpecialVehicleContainer_dangerousGoodsContainer;
                psCam->cam.camParameters.specialVehicleContainer.u.dangerousGoodsContainer.dangerousGoodsBasic = (DangerousGoodsBasic)g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_eDangerousGoodsType;

                psDenm->denm.alacarte_option = TRUE;
                psDenm->denm.alacarte.stationaryVehicle_option = TRUE;
                psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods_option = TRUE;
                psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.dangerousGoodsType = (DangerousGoodsBasic)g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_eDangerousGoodsType;
                psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName_option = TRUE;
                psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName.buf = (uint8_t*)g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleDriverName;
                psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName.len = strlen(g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleDriverName) + 1;
            }

            // Set DENM event.
            psDenm->denm.situation_option = TRUE;
            psDenm->denm.situation.eventType.causeCode = CauseCodeType_commercialVehicleSituation;
            psDenm2->denm.situation_option = TRUE;
            psDenm2->denm.situation.eventType.causeCode = CauseCodeType_slowVehicle;

            // Set vehicle id.
            psDenm->denm.alacarte_option = TRUE;
            psDenm->denm.alacarte.stationaryVehicle_option = TRUE;
            /*
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification_option = TRUE;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber_option = TRUE;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber.buf = (uint8_t*)g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleDriverName;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber.len = strlen(g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleDriverName) + 1;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS_option = TRUE;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS.buf = (uint8_t*)g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleLicensePlate;
            psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS.len = strlen(g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleLicensePlate) + 1;
            */

            // Set denm event locations.
            psDenm->denm.management.eventPosition.latitude = (int32_t)(g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDepartureLocation.m_dLatitude * 10000000.0); /* Convert to 1/10 micro degree. */
            psDenm->denm.management.eventPosition.longitude = (int32_t)(g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDepartureLocation.m_dLongitude * 10000000.0); /* Convert to 1/10 micro degree. */

            psDenm2->denm.management.eventPosition.latitude = (int32_t)(g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDestinationLocation.m_dLatitude * 10000000.0); /* Convert to 1/10 micro degree. */
            psDenm2->denm.management.eventPosition.longitude = (int32_t)(g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_sDestinationLocation.m_dLongitude * 10000000.0); /* Convert to 1/10 micro degree. */
            psDenm2->denm.alacarte_option = FALSE; // TODO Temp code.
                psDenm2->denm.alacarte_option = TRUE;
                psDenm2->denm.alacarte.stationaryVehicle_option = TRUE;
                psDenm2->denm.alacarte.stationaryVehicle.carryingDangerousGoods_option = TRUE;
                psDenm2->denm.alacarte.stationaryVehicle.carryingDangerousGoods.dangerousGoodsType = (DangerousGoodsBasic)g_sLocalStationInfo.m_sVehicleInfo.m_usSpecifics.m_sCommercialVehicleInfo.m_eDangerousGoodsType;
                psDenm2->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName_option = TRUE;
                psDenm2->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName.buf = (uint8_t*)g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleLicensePlate;
                psDenm2->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName.len = strlen(g_sLocalStationInfo.m_sVehicleInfo.m_achVehicleLicensePlate) + 1;

            //printf("Pushing its messages\n");

            its_msg_processor_push_cam_msg(psCam);
            its_msg_processor_push_denm_msg(psDenm);
            its_msg_processor_push_denm_msg(psDenm2);
        }
    }

    return n32ProcedureResult;
}

int32_t its_msg_processor_cm_process_rx_denm(DENM *psDenm, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData) {

    return PROCEDURE_SUCCESSFULL;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

int32_t parameters_info_ini_loader(void* pchUser, const char* pchSection, const char* pchName, const char* pchValue) {

    return PROCEDURE_SUCCESSFULL;
}
