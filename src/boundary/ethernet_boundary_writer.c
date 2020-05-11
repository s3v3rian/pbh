#include "ethernet_boundary_writer.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "common/globals.h"
#include "common/utils/geo_utils.h"

#include "services/gps/nmea_infra.h"

/*
 *******************************************************************************
 * Private function signatures
 *******************************************************************************
 */

static uint32_t convert_char_ip_to_integer(const char *pchIpAddress);

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

static int32_t m_n32SocketFd = 0;

static struct sockaddr_in m_sClientAddress;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t ethernet_boundary_init(char *pchHostIpAddress) {

    struct sockaddr_in sServerAddress;

    // Creating socket file descriptor
    if(0 > (m_n32SocketFd = socket(AF_INET, SOCK_DGRAM, 0))) {

        perror("socket creation failed");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    memset(&sServerAddress, 0, sizeof(sServerAddress));
    memset(&m_sClientAddress, 0, sizeof(m_sClientAddress));

    // Filling server information
    sServerAddress.sin_family    = AF_INET; // IPv4
    sServerAddress.sin_addr.s_addr = INADDR_ANY;
    sServerAddress.sin_port = htons(ETHERNET_BOUNDARY_HOST_PORT);

    // Bind the socket with the server address
    if(0 > bind(m_n32SocketFd, (const struct sockaddr *)&sServerAddress, sizeof(sServerAddress))) {

        perror("bind failed");
        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    m_sClientAddress.sin_family    = AF_INET; // IPv4
    m_sClientAddress.sin_addr.s_addr = convert_char_ip_to_integer(pchHostIpAddress);
    m_sClientAddress.sin_port = htons(ETHERNET_BOUNDARY_HOST_PORT);

    printf("Initialized ethernet boundary writer - Host: %s:%d\n", pchHostIpAddress, ETHERNET_BOUNDARY_HOST_PORT);

    return PROCEDURE_SUCCESSFULL;
}

int32_t ethernet_boundary_write_sentence(char *pchSentence, int32_t n32SentenceSize) {

    return sendto(
        m_n32SocketFd,
        pchSentence,
        n32SentenceSize,
        0,
        (const struct sockaddr *)&m_sClientAddress,
        sizeof(m_sClientAddress));
}

int32_t ethernet_boundary_write_event(int32_t n32EventId) {

    int32_t n32SentenceSize = 0;
    char achSentence[MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES];

    switch(n32EventId) {

        case CauseCodeType_signalViolation:

            n32SentenceSize += sprintf(
                achSentence,
                "T%d,local_event,signal_violation,\n",
                g_sLocalStationInfo.m_un32StationId);
            break;

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

    return ethernet_boundary_write_sentence(achSentence, n32SentenceSize);
}

int32_t ethernet_boundary_write_poti(fix_data_t *psPotiFixData) {

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

    return ethernet_boundary_write_sentence(achSentence, n32SentenceSize);
}

int32_t ethernet_boundary_write_cam(CAM *psCam) {

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

    return ethernet_boundary_write_sentence(achSentence, n32SentenceSize);
}

int32_t ethernet_boundary_write_denm(DENM *psDenm) {

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

    return ethernet_boundary_write_sentence(achSentence, n32SentenceSize);
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

static uint32_t convert_char_ip_to_integer(const char *pchIpAddress) {

    /* The return value. */
    uint32_t un32IpAddress = 0;
    /* The count of the number of bytes processed. */
    int32_t n32Counter;
    /* A pointer to the next digit to process. */
    const char *pchStart;

    pchStart = pchIpAddress;

    for(n32Counter = 0; n32Counter < 4; n32Counter++) {
        /* The digit being processed. */
        char chCurrentDigit;
        /* The value of this byte. */
        int32_t n32ByteValue = 0;

        while (true) {

            chCurrentDigit = * pchStart;
            pchStart++;
            if (chCurrentDigit >= '0' && chCurrentDigit <= '9') {
                n32ByteValue *= 10;
                n32ByteValue += chCurrentDigit - '0';
            }
            /* We insist on stopping at "." if we are still parsing
               the first, second, or third numbers. If we have reached
               the end of the numbers, we will allow any character. */
            else if ((n32Counter < 3 && chCurrentDigit == '.') || n32Counter == 3) {
                break;
            }
            else {
                return 0xFFFFFFFF;
            }
        }
        if (n32ByteValue >= 256) {
            return 0xFFFFFFFF;
        }
        un32IpAddress *= 256;
        un32IpAddress += n32ByteValue;
    }
    return un32IpAddress;
}
