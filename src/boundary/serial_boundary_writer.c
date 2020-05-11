#include "serial_boundary_writer.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "common/globals.h"
#include "common/utils/geo_utils.h"

#include "services/gps/nmea_infra.h"

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

char m_achDevice[MAX_ITS_STRING_SIZE_IN_BYTES];

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t serial_boundary_init(char *pchDevice) {

    memcpy(m_achDevice, pchDevice, strlen(pchDevice) + 1);

    return PROCEDURE_SUCCESSFULL;
}

int32_t serial_boundary_write_sentence(char *pchSentence, int32_t n32SentenceSize) {

    int32_t n32Fd = open(m_achDevice,/*"/dev/ttyAMA2",*/ O_RDWR);

    if(0 > n32Fd) {

        printf("Failed to open file descriptor for file /dev/ttyAMA2\n");
        return -1;
    }

    int8_t n32Result = 0;

    if(n32SentenceSize != write(n32Fd, pchSentence, n32SentenceSize)) {

        n32Result = -2;
    }

    close(n32Fd);

    return n32Result;
}

int32_t serial_boundary_write_event(int32_t n32EventId) {

    int32_t n32SentenceSize = 0;
    char achSentence[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    switch(n32EventId) {

        case CauseCodeType_collisionRisk:

            n32SentenceSize += sprintf(
                achSentence,
                "T%d,local_event,collision_risk,\n",
                g_sLocalStationInfo.m_un32StationId);
            break;

        default:

            n32SentenceSize += sprintf(
                achSentence,
                "T%d,local_event,nothing,\n",
                g_sLocalStationInfo.m_un32StationId);
            break;
    }

    return serial_boundary_write_sentence(achSentence, n32SentenceSize);
}

int32_t serial_boundary_write_poti(fix_data_t *psPotiFixData) {

    int32_t n32SentenceSize = 0;
    char achSentence[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    memset(achSentence, 0, MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES);

    n32SentenceSize += snprintf(
                achSentence + n32SentenceSize,
                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                "T%d,",
                g_sLocalStationInfo.m_un32StationId);

    // Convert POTI info to NMEA data.
    SNmeaGgaData sGgaData;
    SNmeaRmcData sRmcData;

    memset(&sGgaData, 0, sizeof(sGgaData));
    memset(&sRmcData, 0, sizeof(sRmcData));

    // Get data from POTI.
    nmea_get_rmc_data(psPotiFixData, &sRmcData);

    // Build and send NMEA sentence.
    n32SentenceSize += nmea_build_rmc_msg(&sRmcData, achSentence + n32SentenceSize);

    return serial_boundary_write_sentence(achSentence, n32SentenceSize);
}

int32_t serial_boundary_write_cam(CAM *psCam) {

    int32_t n32SentenceSize = 0;
    char achSentence[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    memset(achSentence, 0, MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES);

    n32SentenceSize += snprintf(
                achSentence + n32SentenceSize,
                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                "T%d,",
                psCam->header.stationID);

    SNmeaRmcData sRmcData;
    memset(&sRmcData, 0, sizeof(sRmcData));

    nmea_get_rmc_Data(&sRmcData);

    sRmcData.m_dLatitude = psCam->cam.camParameters.basicContainer.referencePosition.latitude;
    sRmcData.m_dLatitude = sRmcData.m_dLatitude / 10000000.0;
    sRmcData.m_dLatitude = geodesic_convert_decimal_degress_to_degrees_minutes(sRmcData.m_dLatitude);

    sRmcData.m_dLongitude = psCam->cam.camParameters.basicContainer.referencePosition.longitude;
    sRmcData.m_dLongitude = sRmcData.m_dLongitude / 10000000.0;
    sRmcData.m_dLongitude = geodesic_convert_decimal_degress_to_degrees_minutes(sRmcData.m_dLongitude);

    sRmcData.m_dVelcoityInKnots = psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedValue * 1.944;

    n32SentenceSize += nmea_build_rmc_msg(&sRmcData, achSentence + n32SentenceSize);

    return serial_boundary_write_sentence(achSentence, n32SentenceSize);
}

int32_t serial_boundary_write_denm(DENM *psDenm) {

    int32_t n32SentenceSize = 0;
    char achSentence[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    memset(achSentence, 0, MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES);

    // Always write station id first.
    n32SentenceSize += snprintf(
                achSentence + n32SentenceSize,
                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                "T%d",
                psDenm->header.stationID);

    // Write to boundary current situation event.
    if(TRUE == psDenm->denm.situation_option) {

        switch(psDenm->denm.situation.eventType.causeCode) {

            case CauseCodeType_commercialVehicleSituation:

                n32SentenceSize += snprintf(
                            achSentence + n32SentenceSize,
                            MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                            ",remote_event,commercial_vehicle_status");
                break;

            case CauseCodeType_collisionRisk:

                n32SentenceSize += snprintf(
                            achSentence + n32SentenceSize,
                            MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                            ",remote_event,collision _risk");
                break;

            case CauseCodeType_signalViolation:

                if(SignalViolation_trafficLightViolation == psDenm->denm.situation.eventType.subCauseCode) {

                    n32SentenceSize += snprintf(
                                achSentence + n32SentenceSize,
                                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                                ",remote_event,signal_violation");
                }
                break;

            case CauseCodeType_trafficCondition:

                if(TrafficConditionSubCauseCode_trafficRedLight == psDenm->denm.situation.eventType.subCauseCode) {

                    n32SentenceSize += snprintf(
                                achSentence + n32SentenceSize,
                                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                                ",remote_event,red_light");

                } else if(TrafficConditionSubCauseCode_trafficGreenLight == psDenm->denm.situation.eventType.subCauseCode) {

                    n32SentenceSize += snprintf(
                                achSentence + n32SentenceSize,
                                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                                ",remote_event,green_light");
                }
                break;

        case CauseCodeType_slowVehicle:

                n32SentenceSize += snprintf(
                            achSentence + n32SentenceSize,
                            MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                            ",remote_event,slow_vehicle");
                break;

            default:

                n32SentenceSize += snprintf(
                            achSentence + n32SentenceSize,
                            MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                            ",remote_event,%d", psDenm->denm.situation.eventType.causeCode);
                break;
        }

    } else {

        n32SentenceSize += snprintf(
                    achSentence + n32SentenceSize,
                    MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                    ",No event");
    }

    // Write to boundary alacarte container stuff.
    if(TRUE == psDenm->denm.alacarte_option) {

        n32SentenceSize += snprintf(
            achSentence + n32SentenceSize,
            MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
            ",Alacarte");

        // Write to boundary stationary vehicle stuff.
        if(TRUE == psDenm->denm.alacarte.stationaryVehicle_option) {

            // Write to boundary vehicle identification stuff.
            if(TRUE == psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification_option) {

                if(TRUE == psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS_option) {

                    n32SentenceSize += snprintf(
                                achSentence + n32SentenceSize,
                                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                                ",VDS,%s",
                                psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.vDS.buf);
                }

                if(TRUE == psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber_option) {

                    n32SentenceSize += snprintf(
                                achSentence + n32SentenceSize,
                                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                                ",wMInumber,%s",
                                psDenm->denm.alacarte.stationaryVehicle.vehicleIdentification.wMInumber.buf);
                }
            }
        }

        if(TRUE == psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods_option) {

            n32SentenceSize += snprintf(
                        achSentence + n32SentenceSize,
                        MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                        ",IsCarryDangerous,true,Type,%d,CompanyName,%s",
                        psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.dangerousGoodsType,
                        psDenm->denm.alacarte.stationaryVehicle.carryingDangerousGoods.companyName.buf);

        } else {

            n32SentenceSize += snprintf(
                        achSentence + n32SentenceSize,
                        MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                        ",IsCarryDangerous,false");
        }
    }

    n32SentenceSize += snprintf(
                achSentence + n32SentenceSize,
                MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES - n32SentenceSize,
                ",Lat,%d,Lon,%d\n",
                psDenm->denm.management.eventPosition.latitude,
                psDenm->denm.management.eventPosition.longitude);

    return serial_boundary_write_sentence(achSentence, n32SentenceSize);
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */
