#ifndef BOUNDARY_SERIAL_OUTPUT_H_
#define BOUNDARY_SERIAL_OUTPUT_H_

#include <stdint.h>

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

int32_t serial_output_write(char *pchSentence, int32_t n32SentenceSize, const char *pchID);

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif

