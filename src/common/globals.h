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
// ------------------ Container Definitions ----------------
// ---------------------------------------------------------

#define MAX_CONTAINER_NAME_STRING_SIZE_IN_BYTES	20
#define MAX_NUMBER_OF_CONTAINERS		20
#define MAX_NUMBER_OF_CONTAINERS_ELEMENTS	100
#define INVALID_CONTAINER_ID			-1
#define INVALID_CONTAINER_ELEMENT_ID		-1

// ---------------------------------------------------------
// ---------- General Procedure Result Definitions ---------
// ---------------------------------------------------------

#define PROCEDURE_SUCCESSFULL			0
#define PROCEDURE_INVALID_PARAMETERS_ERROR	-1
#define PROCEDURE_INVALID_SERVICE_INIT_ERROR  	-2
#define PROCEDURE_INVALID_SERVICE_TX_ERROR  	-3
#define PROCEDURE_INVALID_SERVICE_RX_ERROR  	-4
#define PROCEDURE_BUFFER_ERROR  		-5
#define PROCEDURE_SECURITY_ERROR		-6

// ---------------------------------------------------------
// ------------- Configuration File Definitions ------------
// ---------------------------------------------------------

#define CONFIGURATION_FILE_STATION_INFO_USER	"STATION_INFO_USER"
#define CONFIGURATION_FILE_SCENARIO_PARAMS_USER	"SCENARIO_INFO_USER"
#define CONFIGURATION_FILE_GENERAL_PARAMS_USER	"GENERAL_PARAMS_USER"
#define CONFIGURATION_FILE_SCENARIO_PARTICIPANT_PARAMS_USER	"SCENARIO_PARTICIPANT_USER"

// ---------------------------------------------------------
// ------------------ Message Definitions ------------------
// ---------------------------------------------------------

#define INVALID_EVENT_ID			0xFF
#define POTI_id					10
#define MAX_ITS_STRING_SIZE_IN_BYTES		20
#define MAX_BOUNDARY_SENTENCE_SIZE_IN_BYTES	1024

/*
 *******************************************************************************
 * Macros
 *******************************************************************************
 */

// #define LOG(fmt, ...) printf("%s:%d - %s", __FILE__, __LINE__, fmt, __VA_ARGS__);
#define DEBUG_LOG(fmt, ...) \
    do { if (__DEBUG_PRINTING_ENABLED__) fprintf(stdout, "%s:%d:%s() - ", fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while(0)

/*
 *******************************************************************************
 * Data type definition
 *******************************************************************************
 */

// Enumerators.

// Callbacks.
typedef int32_t (*boundary_write)(char *pchSentence, int32_t n32SentenceSize, uint32_t un32StationId);

// ---------------------------------------------------
// ------------------ SA Structures ------------------
// ---------------------------------------------------

// Structures.
typedef struct SStationLLAData {

    double m_dLongitude;
    double m_dLatitude;
    double m_dAltitude;

} SStationLLAData;

typedef struct SStationDynamicData {

    double m_dCurrentHaversine;

} SStationDynamicData;

typedef struct SStationFinalizedEventData {

    uint64_t m_un64Events;

} SStationFinalizedEventData;

typedef struct SITSStationFusionData {

    uint32_t m_un32StationId;
    int32_t m_n32StationType;

    double m_dLastPotiTAI;

    SStationLLAData m_sCurrentLLAData;
    SStationFinalizedEventData m_sCurrentEventData;
    SStationDynamicData m_sCurrentDynamicData;

} SITSStationFusionData;

// ---------------------------------------------------
// --------------- Station Structures ----------------
// ---------------------------------------------------

typedef struct SITSCommercialStationInfo {

    SStationLLAData m_sDepartureLocation;
    SStationLLAData m_sDestinationLocation;

    bool m_bIsDangerousGoods;
    int32_t m_eDangerousGoodsType;

    bool m_bIsTrailerAttached;
    int32_t m_n32TrailerLength;
    int32_t m_n32TrailerHeight;
    int32_t m_n32TrailerWidth;
    int32_t m_n32TrailerWeight;

} SITSCommercialStationInfo;

typedef struct SITSVehicleStationInfo {

    char m_achVehicleDriverName[MAX_ITS_STRING_SIZE_IN_BYTES];
    char m_achVehicleLicensePlate[MAX_ITS_STRING_SIZE_IN_BYTES];

    // Dimensions.
    int32_t m_n32VehicleLength;
    int32_t m_n32VehicleWidth;
    int32_t m_n32VehicleHeight;
    int32_t m_n32VehicleWeight;
    double m_dVehicleSpeed;

    union {

        SITSCommercialStationInfo m_sCommercialVehicleInfo;

    } m_usSpecifics;

} SITSVehicleStationInfo;

typedef struct SITSStationGeneralParameters {

    int32_t m_n32TxFrequencyIn10Hz;

} SITSStationGeneralParameters;

typedef struct SITSStationInfo {

    uint32_t m_un32StationId;
    int32_t m_n32StationType;
    int32_t m_n32SubStationType;

    SITSStationGeneralParameters m_sParameters;
    SITSVehicleStationInfo m_sVehicleInfo;

} SITSStationInfo;

// ---------------------------------------------------
// -------------- Container Structures ---------------
// ---------------------------------------------------

typedef struct SDataContainerElement {

    int32_t m_n32Data;
    char *m_pchData;

} SDataContainerElement;

// ---------------------------------------------------
// -------------- Simulator Structures ---------------
// ---------------------------------------------------

typedef struct SITSScenarioInfo {

    // Scenario info.
    bool m_bIsScenarioEnabled;
    char m_achName[30];
    bool m_bIsGpsSimEnabled;
    char m_achParticipantId[30];
    uint32_t m_un32GpSimSyncId;

    // Participant info.
    char m_achParticipantName[30];

} SITSScenarioInfo;

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

// Global configuration.
SITSStationInfo g_sLocalStationInfo;
SITSScenarioInfo g_sLocalScenarioInfo;
char *g_pchConfigurationFileDirectoryPath;

// Callbacks.
boundary_write g_fp_write_to_boundary;

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


