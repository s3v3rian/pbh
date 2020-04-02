#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdint.h>

#include "itsmsg.h"
#include "itsmsg_codec.h"
#include "poti_service.h"
#include "gn_service_user.h"
#include "error_code_user.h"

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

#define	BOOLEAN_TRUE				1
#define BOOLEAN_FALSE				0

#define PROCEDURE_SUCCESSFULL			0
#define PROCEDURE_INVALID_PARAMETERS_ERROR	-1
#define PROCEDURE_INVALID_SERVICE_INIT_ERROR  	-2
#define PROCEDURE_INVALID_SERVICE_TX_ERROR  	-3
#define PROCEDURE_INVALID_SERVICE_RX_ERROR  	-4
#define PROCEDURE_SECURITY_ERROR		-5

#define SENDER_FREQUENCY_IN_10_HZ		1

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

static btp_handler_ptr pBtpCamHandler;
static btp_handler_ptr pBtpDenmHandler;

/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */

#endif


