#ifndef COMMON_GLOBALS_H_
#define COMMON_GLOBALS_H_

#include <stdint.h>
#include <stdlib.h>

/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

// ---------------------------------------------------------
// --------------- General Status Definitions --------------
// ---------------------------------------------------------

#define	BOOLEAN_TRUE				1
#define BOOLEAN_FALSE				0

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

typedef struct SITSStationInfo {

    uint32_t m_un32StationId;
    int32_t m_n32StationType;

} SITSStationInfo;

typedef struct SITSScenarioInfo {

    int32_t m_n32IsScenarioEnabled;
    char m_achName[30];
    int32_t m_n32IsGpsSimEnabled;
    char m_achParticipantId[30];
    uint32_t m_un32GpSimSyncId;

} SITSScenarioInfo;

typedef struct SITSTxParameters {

    int32_t m_n32TxFrequencyIn10Hz;

} SITSTxParameters;

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

SITSStationInfo g_sStationInfo;
SITSScenarioInfo g_sScenarioInfo;
SITSTxParameters g_sTxParameters;

char *g_pchConfigurationFileDirectoryPath;

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


