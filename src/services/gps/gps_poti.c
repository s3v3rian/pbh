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

static fix_data_t m_asPotiRingBuffer[MAX_NUMBER_OF_CONTAINERS_ELEMENTS];

static int32_t m_n32PotiRingBufferIndex = 0;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t gps_poti_init() {

    // Initialize POTI service.
    int n32ProcedureResult = poti_create_service(&m_pPotiHandler, NULL);

    if(FALSE == IS_SUCCESS(n32ProcedureResult)) {

        return PROCEDURE_INVALID_SERVICE_INIT_ERROR;
    }

    m_n32PotiRingBufferIndex = 0;

    return n32ProcedureResult;
}

int32_t gps_poti_get_fix_data(fix_data_t **p2sPotiFixData) {

    int32_t n32ProcedureResult = PROCEDURE_SUCCESSFULL;

    /* Wait for new GNSS update (10 Hz by default). */
    n32ProcedureResult = poti_wait_gnss_data(m_pPotiHandler, NULL);

    if(false == IS_SUCCESS(n32ProcedureResult)) {

        printf("poti_wait_gnss_data error: %s\n", ERROR_MSG(n32ProcedureResult));
        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    *p2sPotiFixData = &m_asPotiRingBuffer[m_n32PotiRingBufferIndex];
    m_n32PotiRingBufferIndex = (m_n32PotiRingBufferIndex + 1) % MAX_NUMBER_OF_CONTAINERS_ELEMENTS;

    // Get fix data.
    n32ProcedureResult = poti_get_fix_data(m_pPotiHandler, *p2sPotiFixData);

    if(false == IS_SUCCESS(n32ProcedureResult)) {

        printf("Cannot get GNSS fix data: %s\n", ERROR_MSG(n32ProcedureResult));
        return PROCEDURE_INVALID_SERVICE_RX_ERROR;
    }

    return n32ProcedureResult;
}

void gps_poti_release() {

    poti_release_service(m_pPotiHandler);
}
