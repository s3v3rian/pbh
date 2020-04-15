#include "sa_mngr.h"

#include "gps/nmea_infra.h"
#include "boundary/serial_output.h"

void sa_process_services() {

}

int32_t sa_process_poti_data(fix_data_t *psPotiFixData) {

    // Convert POTI info to NMEA data.
    SNmeaGgaData sGgaData;
    SNmeaRmcData sRmcData;
    char achNmeaSentence[300];
    int32_t n32SentenceSize = 0;

    memset(&sGgaData, 0, sizeof(sGgaData));
    memset(&sRmcData, 0, sizeof(sRmcData));

    // Get data from POTI.
    nmea_get_gga_data(psPotiFixData, &sGgaData);
    nmea_get_rmc_data(psPotiFixData, &sRmcData);

    // Build and send NMEA sentence.
    n32SentenceSize = nmea_build_gga_msg(&sGgaData, achNmeaSentence);
    serial_output_write(achNmeaSentence, n32SentenceSize, g_sStationInfo.m_un32StationId);

    // Build and send NMEA sentence.
    n32SentenceSize = nmea_build_rmc_msg(&sRmcData, achNmeaSentence);
    serial_output_write(achNmeaSentence, n32SentenceSize, g_sStationInfo.m_un32StationId);

    return PROCEDURE_SUCCESSFULL;
}

int32_t sa_process_cam_data(CAM *psCam) {

    SNmeaRmcData sRmcData;
    char achSentence[300];
    int32_t n32SentenceSize = 0;

    memset(&sRmcData, 0, sizeof(sRmcData));

    nmea_get_rmc_Data(&sRmcData);

    sRmcData.m_dLatitude = psCam->cam.camParameters.basicContainer.referencePosition.latitude;
    sRmcData.m_dLatitude = sRmcData.m_dLatitude / 10000000;
    sRmcData.m_dLatitude = nmea_convert_decimal_degress_to_degrees_minutes(sRmcData.m_dLatitude);

    sRmcData.m_dLongitude = psCam->cam.camParameters.basicContainer.referencePosition.longitude;
    sRmcData.m_dLongitude = sRmcData.m_dLongitude / 10000000;
    sRmcData.m_dLongitude = nmea_convert_decimal_degress_to_degrees_minutes(sRmcData.m_dLongitude);

    sRmcData.m_dVelcoityInKnots = psCam->cam.camParameters.highFrequencyContainer.u.basicVehicleContainerHighFrequency.speed.speedValue * 1.944;

    n32SentenceSize = nmea_build_rmc_msg(&sRmcData, achSentence);
    serial_output_write(achSentence, n32SentenceSize, psCam->header.stationID);

    return PROCEDURE_SUCCESSFULL;
}

int32_t sa_process_denm_data(DENM *psDenm) {

    if(TRUE == psDenm->denm.situation_option) {

        char achSentence[300];
        int32_t n32SentenceSize = 0;

        switch(psDenm->denm.situation.eventType.causeCode) {

            case CauseCodeType_redLight:

                // Received a red light from RSU.
                n32SentenceSize = sprintf(achSentence, "EVENT,RED_LIGHT\n");
                break;

            default:

                n32SentenceSize = sprintf(achSentence, "EVENT,%d\n", psDenm->denm.situation.eventType.causeCode);
                break;
        }

        serial_output_write(achSentence, n32SentenceSize, psDenm->header.stationID);
    }

    return PROCEDURE_SUCCESSFULL;
}
