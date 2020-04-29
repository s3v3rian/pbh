#ifndef SERVICES_MAPEM_MAPEM_MNGR_H_
#define SERVICES_MAPEM_MAPEM_MNGR_H_

#include "lib/inc/itsmsg.h"
#include "lib/inc/itsmsg_codec.h"
#include "lib/inc/itsmsg_mapem.h"
#include "lib/inc/poti_service.h"
#include "lib/inc/gn_service_user.h"
#include "globals.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

#define MAPEM_BTP_PORT                                2003    /* ETSI TS 103 248. */

#define MAPEM_PROTOCOL_VERSION                        1       /* ETSI EN 302 637-2 V1.3.1 (2014-09) */

/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */

#define MAPEM_SSP_CHECK(x,y) (((x) & (y)) ? 1 : 0)
#define MAPEM_SSP_LEN 3

/*
 *******************************************************************************
 * Data type definition
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t mapem_mngr_init();
int32_t mapem_mngr_process_tx(SITSStationInfo *psStationInfo, fix_data_t *psPotiFixData, MAPEM *psOutputMapem);
int32_t mapem_mngr_process_rx(MAPEM *psOutputMapem);
int32_t mapem_mngr_release();

#endif


