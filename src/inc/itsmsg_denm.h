/* Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef __ITSMSG_DENM_H__
#define __ITSMSG_DENM_H__

#include "itsmsg_cdd.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Termination {
    Termination_isCancellation,
    Termination_isNegation,
    Termination_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} Termination;

typedef struct ManagementContainer {
    ActionID actionID;
    BigInteger detectionTime; /* TimestampIts */
    BigInteger referenceTime; /* TimestampIts */
    BOOL termination_option;
    Termination termination;
    ReferencePosition eventPosition;
    BOOL relevanceDistance_option;
    RelevanceDistance relevanceDistance;
    BOOL relevanceTrafficDirection_option;
    RelevanceTrafficDirection relevanceTrafficDirection;
    int32_t validityDuration; /* ValidityDuration (0..86400) */
    BOOL transmissionInterval_option;
    int32_t transmissionInterval; /* TransmissionInterval (1..10000) */
    int32_t stationType; /* StationType (0..255) */
} ManagementContainer;

typedef struct SituationContainer {
    int32_t informationQuality; /* InformationQuality (0..7) */
    CauseCode eventType;
    BOOL linkedCause_option;
    CauseCode linkedCause;
    BOOL eventHistory_option;
    EventHistory eventHistory;
} SituationContainer;

typedef struct LocationContainer {
    BOOL eventSpeed_option;
    Speed eventSpeed;
    BOOL eventPositionHeading_option;
    Heading eventPositionHeading;
    Traces traces;
    BOOL roadType_option;
    RoadType roadType;
} LocationContainer;

typedef struct ImpactReductionContainer {
    int32_t heightLonCarrLeft; /* HeightLonCarr (1..100) */
    int32_t heightLonCarrRight; /* HeightLonCarr (1..100) */
    int32_t posLonCarrLeft; /* PosLonCarr (1..127) */
    int32_t posLonCarrRight; /* PosLonCarr (1..127) */
    PositionOfPillars positionOfPillars;
    int32_t posCentMass; /* PosCentMass (1..63) */
    int32_t wheelBaseVehicle; /* WheelBaseVehicle (1..127) */
    int32_t turningRadius; /* TurningRadius (1..255) */
    int32_t posFrontAx; /* PosFrontAx (1..20) */
    BitString positionOfOccupants; /* PositionOfOccupants (20..20) */
    int32_t vehicleMass; /* VehicleMass (1..1024) */
    RequestResponseIndication requestResponseIndication;
} ImpactReductionContainer;

#define ReferenceDenms_MAX_SIZE 8
typedef struct ReferenceDenms {
    ActionID *tab;
    int32_t count;
} ReferenceDenms;

typedef struct RoadWorksContainerExtended {
    BOOL lightBarSirenInUse_option;
    BitString lightBarSirenInUse; /* LightBarSirenInUse (2..2) */
    BOOL closedLanes_option;
    ClosedLanes closedLanes;
    BOOL restriction_option;
    RestrictedTypes restriction;
    BOOL speedLimit_option;
    int32_t speedLimit; /* SpeedLimit (1..255) */
    BOOL incidentIndication_option;
    CauseCode incidentIndication;
    BOOL recommendedPath_option;
    ItineraryPath recommendedPath;
    BOOL startingPointSpeedLimit_option;
    DeltaReferencePosition startingPointSpeedLimit;
    BOOL trafficFlowRule_option;
    TrafficRule trafficFlowRule;
    BOOL referenceDenms_option;
    ReferenceDenms referenceDenms;
} RoadWorksContainerExtended;

typedef struct StationaryVehicleContainer {
    BOOL stationarySince_option;
    StationarySince stationarySince;
    BOOL stationaryCause_option;
    CauseCode stationaryCause;
    BOOL carryingDangerousGoods_option;
    DangerousGoodsExtended carryingDangerousGoods;
    BOOL numberOfOccupants_option;
    int32_t numberOfOccupants; /* NumberOfOccupants (0..127) */
    BOOL vehicleIdentification_option;
    VehicleIdentification vehicleIdentification;
    BOOL energyStorageType_option;
    BitString energyStorageType; /* EnergyStorageType (7..7) */
} StationaryVehicleContainer;

typedef struct AlacarteContainer {
    BOOL lanePosition_option;
    int32_t lanePosition; /* LanePosition (-1..14) */
    BOOL impactReduction_option;
    ImpactReductionContainer impactReduction;
    BOOL externalTemperature_option;
    int32_t externalTemperature; /* Temperature (-60..67) */
    BOOL roadWorks_option;
    RoadWorksContainerExtended roadWorks;
    BOOL positioningSolution_option;
    PositioningSolutionType positioningSolution;
    BOOL stationaryVehicle_option;
    StationaryVehicleContainer stationaryVehicle;
} AlacarteContainer;

typedef struct DecentralizedEnvironmentalNotificationMessage {
    ManagementContainer management;
    BOOL situation_option;
    SituationContainer situation;
    BOOL location_option;
    LocationContainer location;
    BOOL alacarte_option;
    AlacarteContainer alacarte;
} DecentralizedEnvironmentalNotificationMessage;

typedef struct DENM {
    ItsPduHeader header;
    DecentralizedEnvironmentalNotificationMessage denm;
} DENM;


#ifdef __cplusplus
}
#endif

#endif /* __ITSMSG_DENM_H__ */
