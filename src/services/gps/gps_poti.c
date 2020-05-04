#include "gps_poti.h"

#include "lib/inc/error_code_user.h"
#include "lib/inc/asn1defs_if.h"

#include "services/gps/nmea_infra.h"

/*
 *******************************************************************************
 * Private variables
 *******************************************************************************
 */

poti_service_t *m_pPotiHandler;

fix_data_t m_asPotiRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];

int32_t m_n32PotiRingBufferIndex = 0;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t gps_poti_init() {

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    // Initialize POTI service.
    n32ProcedureResult = IS_SUCCESS(poti_create_service(&m_pPotiHandler, NULL));

    if(FALSE == IS_SUCCESS(n32ProcedureResult)) {

        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    return n32ProcedureResult;

}

int32_t gps_poti_get_fix_data(fix_data_t **p2sPotiFixData) {

    fix_data_t *psPotiFixData = &m_asPotiRingBuffer[m_n32PotiRingBufferIndex++];
    m_n32PotiRingBufferIndex = (m_n32PotiRingBufferIndex % MAX_NUMBER_OF_CONTAINERS_ELEMENTS);

    if(NULL == psPotiFixData) {

        return PROCEDURE_INVALID_PARAMETERS_ERROR;
    }

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    /* Wait for new GNSS update (10 Hz by default). */
    n32ProcedureResult = poti_wait_gnss_data(m_pPotiHandler, NULL);

    if(false == IS_SUCCESS(n32ProcedureResult)) {

        printf("poti_wait_gnss_data error: %s\n", ERROR_MSG(n32ProcedureResult));
        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    // Get fix data.
    n32ProcedureResult = poti_get_fix_data(m_pPotiHandler, psPotiFixData);

    if(false == IS_SUCCESS(n32ProcedureResult)) {

        printf("Cannot get GNSS fix data: %s\n", ERROR_MSG(n32ProcedureResult));
        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    *p2sPotiFixData = psPotiFixData;

    return n32ProcedureResult;
}

void gps_poti_release() {

    poti_release_service(m_pPotiHandler);
}
