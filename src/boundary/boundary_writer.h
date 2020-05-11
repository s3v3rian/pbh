#ifndef COMMON_BOUNDARY_WRITER_H_
#define COMMON_BOUNDARY_WRITER_H_

#include "lib/inc/poti_service.h"
#include "lib/inc/itsmsg_cam.h"
#include "lib/inc/itsmsg_denm.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

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

// Host boundary callback definitions.
typedef int32_t (*host_boundary_write_init)(char *pchHostInfo);
typedef int32_t (*host_boundary_write_sentence)(char *pchSentence, int32_t n32SentenceSize);
typedef int32_t (*host_boundary_write_event)(int32_t n32EventId);
typedef int32_t (*host_boundary_write_poti)(fix_data_t *psPotiFixData);
typedef int32_t (*host_boundary_write_cam)(CAM *psCam);
typedef int32_t (*host_boundary_write_denm)(DENM *psDenm);

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

// Host boundary callbacks.
host_boundary_write_init g_fp_write_to_boundary_init;
host_boundary_write_sentence g_fp_write_to_boundary_sentence;
host_boundary_write_event g_fp_write_to_boundary_event;
host_boundary_write_poti g_fp_write_to_boundary_poti;
host_boundary_write_cam g_fp_write_to_boundary_cam;
host_boundary_write_denm g_fp_write_to_boundary_denm;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

#endif


