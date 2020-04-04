/* J2735 data frame and element defintions
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef __ITSMSG_ISO19091_H__
#define __ITSMSG_ISO19091_H__
#include <limits.h>
#include "asn1defs_if.h"
#include "j2735_data_component.h"
#include "itsmsg_cdd.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    TimeReference_oneMilliSec = 1,
};

typedef struct ItsStationPosition {
    uint32_t stationID; /* StationID (0..4294967295) */
    BOOL laneID_option;
    int32_t laneID; /* LaneID (0..255) */
    BOOL nodeXY_option;
    NodeOffsetPointXY nodeXY;
    BOOL timeReference_option;
    int32_t timeReference; /* TimeReference (0..65535) */
} ItsStationPosition;

#define ItsStationPositionList_MAX_SIZE 5
typedef struct ItsStationPositionList {
    ItsStationPosition *tab;
    int count;
} ItsStationPositionList;

typedef struct ConnectionManeuverAssist_addGrpC {
    BOOL itsStationPosition_option;
    ItsStationPositionList itsStationPosition;
    BOOL rsuGNSSOffset_option;
    NodeOffsetPointXY rsuGNSSOffset;
} ConnectionManeuverAssist_addGrpC;

typedef struct ConnectionTrajectory_addGrpC {
    NodeSetXY nodes;
} ConnectionTrajectory_addGrpC;

typedef struct PrioritizationResponse {
    uint32_t stationID; /* StationID (0..4294967295) */
    PrioritizationResponseStatus priorState;
    int32_t signalGroup; /* SignalGroupID (0..255) */
} PrioritizationResponse;

#define PrioritizationResponseList_MAX_SIZE 10
typedef struct PrioritizationResponseList {
    PrioritizationResponse *tab;
    int count;
} PrioritizationResponseList;

typedef struct IntersectionState_addGrpC {
    BOOL activePrioritizations_option;
    PrioritizationResponseList activePrioritizations;
} IntersectionState_addGrpC;

typedef enum PtvRequestType {
    PtvRequestType_preRequest,
    PtvRequestType_mainRequest,
    PtvRequestType_doorCloseRequest,
    PtvRequestType_cancelRequest,
    PtvRequestType_emergencyRequest,
    PtvRequestType_MAX = UINT_MAX
} PtvRequestType;

typedef struct Control_addGrpC {
    PtvRequestType ptvRequest;
} Control_addGrpC;

typedef struct Position3D_addGrpC {
    Altitude altitude;
} Position3D_addGrpC;

typedef enum EmissionType {
    EmissionType_euro1,
    EmissionType_euro2,
    EmissionType_euro3,
    EmissionType_euro4,
    EmissionType_euro5,
    EmissionType_euro6,
    EmissionType_MAX = UINT_MAX
} EmissionType;

typedef struct RestrictionUserType_addGrpC {
    BOOL emission_option;
    EmissionType emission;
} RestrictionUserType_addGrpC;

typedef struct SignalStatusPackage_addGrpC {
    BOOL synchToSchedule_option;
    int32_t synchToSchedule; /* DeltaTime (-122 .. 121) */
} SignalStatusPackage_addGrpC;

typedef struct SignalHeadLocation {
    NodeOffsetPointXY nodeXY;
    int32_t nodeZ; /* DeltaAltitude (-12700..12800) */
    int32_t signalGroup; /* SignalGroupID (0..255) */
} SignalHeadLocation;

#define SignalHeadLocationList_MAX_SIZE 64
typedef struct SignalHeadLocationList {
    SignalHeadLocation *tab;
    int count;
} SignalHeadLocationList;

typedef struct MapData_addGrpC {
    BOOL signalHeadLocations_option;
    SignalHeadLocationList signalHeadLocations;
} MapData_addGrpC;

#ifdef __cplusplus
}
#endif

#endif /* __ITSMSG_ISO19091_H__ */
