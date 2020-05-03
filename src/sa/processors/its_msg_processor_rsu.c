#include "its_msg_processor_rsu.h"

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

static bool m_bIsDoingDenmStatusUpdate;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t its_msg_processor_rsu_init() {

    printf("Initializing RSU ITS msg processor\n");

    int32_t n32ProcedureResult = its_msg_processor_init();

    m_bIsDoingDenmStatusUpdate = false;

    return n32ProcedureResult;
}

int32_t its_msg_processor_rsu_process_poti(fix_data_t *psPotiFixData) {

    m_bIsDoingDenmStatusUpdate = false;

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_process_rx_cam(CAM *psCam, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData) {

    return PROCEDURE_SUCCESSFULL;
}

int32_t its_msg_processor_rsu_process_rx_denm(DENM *psDenm, SITSStationFusionData *psLocalStationData, SITSStationFusionData *psRemoteStationData) {

    /*
    if(GN_ITS_STATION_HEAVY_TRUCK == psRemoteStationData->m_n32StationType
            || GN_ITS_STATION_LIGHT_TRUCK == psRemoteStationData->m_n32StationType
            || GN_ITS_STATION_PASSENGER_CAR == psRemoteStationData->m_n32StationType
            || GN_ITS_STATION_BUS == psRemoteStationData->m_n32StationType) {

    }
    */

    return PROCEDURE_SUCCESSFULL;
}

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */