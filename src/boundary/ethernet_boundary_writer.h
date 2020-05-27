#ifndef BOUNDARY_ETHERNET_BOUNDARY_H_
#define BOUNDARY_ETHERNET_BOUNDARY_H_

#include <stdint.h>

#include "boundary_writer.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

#define ETHERNET_BOUNDARY_HOST_PORT			12355

/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */

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

int32_t ethernet_boundary_init(char *pchHostIpAddress);
int32_t ethernet_boundary_write_sentence(char *pchSentence, int32_t n32SentenceSize);
int32_t ethernet_boundary_write_event(int32_t n32EventId);
int32_t ethernet_boundary_write_remote_event(int32_t n32EventId, uint32_t un32StationId);
int32_t ethernet_boundary_write_poti(fix_data_t *psPotiFixData);
int32_t ethernet_boundary_write_cam(CAM *psCam);
int32_t ethernet_boundary_write_denm(DENM *psDenm);

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


