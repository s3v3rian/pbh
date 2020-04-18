#ifndef COMMON_GLOBALS_H_
#define COMMON_GLOBALS_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

// ---------------------------------------------------------
// ------------------- Thread Definitions ------------------
// ---------------------------------------------------------

#define MAX_NUMBER_OF_THREADS			8

// ---------------------------------------------------------
// -------------------- Queue Definitions ------------------
// ---------------------------------------------------------

#define MAX_QUEUE_NAME_STRING_SIZE_IN_BYTES	20
#define MAX_NUMBER_OF_QUEUES			20
#define MAX_NUMBER_OF_QUEUE_ELEMENTS		100
#define INVALID_QUEUE_ID			-1
#define INVALID_QUEUE_ELEMENT_ID		-1

// ---------------------------------------------------------
// ---------- General Procedure Result Definitions ---------
// ---------------------------------------------------------

#define PROCEDURE_SUCCESSFULL			0
#define PROCEDURE_INVALID_PARAMETERS_ERROR	-1
#define PROCEDURE_INVALID_SERVICE_INIT_ERROR  	-2
#define PROCEDURE_INVALID_SERVICE_TX_ERROR  	-3
#define PROCEDURE_INVALID_SERVICE_RX_ERROR  	-4
#define PROCEDURE_SECURITY_ERROR		-5

// ---------------------------------------------------------
// ------------- Configuration File Definitions ------------
// ---------------------------------------------------------

#define CONFIGURATION_FILE_STATION_INFO_USER	"STATION_INFO_USER"
#define CONFIGURATION_FILE_SCENARIO_PARAMS_USER	"SCENARIO_INFO_USER"
#define CONFIGURATION_FILE_GENERAL_PARAMS_USER	"GENERAL_PARAMS_USER"

// ---------------------------------------------------------
// ------------------ Message Definitions ------------------
// ---------------------------------------------------------

#define MAX_MSG_RING_BUFFER_SIZE		25
#define POTI_id					0

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

// Callbacks.
typedef int32_t (*boundary_write)(char *pchSentence, int32_t n32SentenceSize, uint32_t un32StationId);

// Structures.
typedef struct SITSStationInfo {

    uint32_t m_un32StationId;
    int32_t m_n32StationType;

} SITSStationInfo;

typedef struct SITSScenarioInfo {

    bool m_bIsScenarioEnabled;
    char m_achName[30];
    bool m_bIsGpsSimEnabled;
    char m_achParticipantId[30];
    uint32_t m_un32GpSimSyncId;

} SITSScenarioInfo;

typedef struct SITSTxParameters {

    int32_t m_n32TxFrequencyIn10Hz;

} SITSTxParameters;

typedef struct SQueueElement {

    int32_t n32Data;
    char *m_pchData;

} SQueueElement;

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

// Global configuration.
SITSStationInfo g_sStationInfo;
SITSScenarioInfo g_sScenarioInfo;
SITSTxParameters g_sTxParameters;
char *g_pchConfigurationFileDirectoryPath;

// Callbacks.
boundary_write g_fpBoundaryWriter;

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


