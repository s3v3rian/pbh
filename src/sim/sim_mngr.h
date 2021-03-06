#ifndef SIM_SIM_MNGR_H_
#define SIM_SIM_MNGR_H_

#include "lib/inc/poti_service.h"

#include "common/globals.h"

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

// ---------------------------------------
// ---- SIMULATOR PROCESSOR CALLBACKS ----
// ---------------------------------------

typedef int32_t (*sim_processor_init)();
typedef void (*sim_processor_do)();
typedef void (*sim_processor_gui)();

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

// ---------------------------------------
// ---- SIMULATOR PROCESSOR CALLBACKS ----
// ---------------------------------------
sim_processor_init g_fp_sim_processor_init;
sim_processor_do g_fp_sim_processor_do_fusion;
sim_processor_gui g_fp_sim_processor_gui;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

int32_t sim_mngr_init();
void sim_mngr_release();
void sim_mngr_process_fusion();
void sim_mngr_process_fix_data(fix_data_t *psPotiFixData);


#endif


