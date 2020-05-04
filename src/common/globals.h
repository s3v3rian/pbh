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

#define CONFIGURATION_FILE_STATION_INFO_USER			"STATION_INFO_USER"
#define CONFIGURATION_FILE_SCENARIO_PARAMS_USER			"SCENARIO_INFO_USER"
#define CONFIGURATION_FILE_GENERAL_PARAMS_USER			"GENERAL_PARAMS_USER"
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

// ---------------------------------------------------
// ------------------- Enumerators -------------------
// ---------------------------------------------------

// ---------------------------------------------------
// -------------- Callback Definitions ---------------
// ---------------------------------------------------

// Host controller callback definitions.
typedef void (*host_controller)();

// ---------------------------------------------------
// ------------------ SA Structures ------------------
// ---------------------------------------------------

typedef struct SDistanceData {

    double m_dLongitude;
    double m_dLatitude;
    double m_dAltitude;
    double m_dDistanceToLocalInMeters;

} SDistanceData;

typedef struct SMovementData {

    double m_dCurrentSpeedInKph;

} SMovementData;

typedef struct SStationFullFusionData {

    uint32_t m_un32StationId;
    int32_t m_n32StationType;

    double m_dLastPotiTAI; // Relevant to local station.

    SDistanceData m_sDistanceData;
    SMovementData m_sMovementData;

} SStationFullFusionData;

// ---------------------------------------------------
// ------------- RSU Station Structures --------------
// ---------------------------------------------------

typedef struct STrafficLightRsuStationInfo {

    bool m_bIsRedLight;
    int32_t m_n32SignalViolationThresholdInMeters;

} STrafficLightRsuStationInfo;

typedef struct SRsuStationInfo {

    union {

        STrafficLightRsuStationInfo m_sTrafficLightInfo;

    } m_usSpecifics;

} SRsuStationInfo;

// ---------------------------------------------------
// ----------- Vehicle Station Structures ------------
// ---------------------------------------------------

typedef struct SCommercialVehicleStationInfo {

    SDistanceData m_sDepartureLocation;
    SDistanceData m_sDestinationLocation;

    bool m_bIsDangerousGoods;
    int32_t m_eDangerousGoodsType;

    bool m_bIsTrailerAttached;
    int32_t m_n32TrailerLength;
    int32_t m_n32TrailerHeight;
    int32_t m_n32TrailerWidth;
    int32_t m_n32TrailerWeight;

} SCommercialVehicleStationInfo;

typedef struct SVehicleStationInfo {

    char m_achVehicleDriverName[MAX_ITS_STRING_SIZE_IN_BYTES];
    char m_achVehicleLicensePlate[MAX_ITS_STRING_SIZE_IN_BYTES];

    // Dimensions.
    int32_t m_n32VehicleLength;
    int32_t m_n32VehicleWidth;
    int32_t m_n32VehicleHeight;
    int32_t m_n32VehicleWeight;
    double m_dVehicleSpeedInKph;

    double m_dCollisionWarningThresholdInMeters;

    union {

        SCommercialVehicleStationInfo m_sCommercialVehicleInfo;

    } m_usSpecifics;

} SVehicleStationInfo;

// ---------------------------------------------------
// ------------ Common Station Structures ------------
// ---------------------------------------------------

typedef struct SStationGeneralParameters {

    int32_t m_n32TxFrequencyIn10Hz;

} SStationGeneralParameters;

typedef struct SStationInfo {

    uint32_t m_un32StationId;
    int32_t m_n32StationType;
    int32_t m_n32SubStationType;

    SStationGeneralParameters m_sParameters;

    SVehicleStationInfo m_sVehicleInfo;
    SRsuStationInfo m_sRsuInfo;

} SStationInfo;

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

typedef struct SSimulatorVehicleInfo {

    double m_dVehicleSpeedDeltaInKph;
    int32_t m_dVehicleSpeedDeltaInMs;

} SSimulatorVehicleInfo;

typedef struct SSimulatorScenarioInfo {

    // General Scenario info.
    bool m_bIsScenarioEnabled;
    char m_achScenarioName[30];

    // GPS Simulator info.
    char m_achParticipantId[30];
    uint32_t m_un32GpSimSyncId;

    SSimulatorVehicleInfo m_sVehicleInfo;

} SSimulatorScenarioInfo;

/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */

// Global configuration.
SStationInfo g_sLocalStationInfo;
SSimulatorScenarioInfo g_sLocalScenarioInfo;
char *g_pchConfigurationFileDirectoryPath;

// Host controller callbacks.
host_controller g_fp_access_host_controller;

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


