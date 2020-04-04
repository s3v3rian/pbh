/* Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef __ITSMSG_CAM_H__
#define __ITSMSG_CAM_H__

#include "itsmsg_cdd.h"

#ifdef __cplusplus
extern "C" {
#endif

enum { /* Pre-defined values of GenerationDeltaTime */
       GenerationDeltaTime_oneMilliSec = 1,
};

typedef struct BasicContainer {
    int32_t stationType; /* StationType (0..255) */
    ReferencePosition referencePosition;
} BasicContainer;

typedef struct BasicVehicleContainerHighFrequency {
    Heading heading;
    Speed speed;
    DriveDirection driveDirection;
    VehicleLength vehicleLength;
    int32_t vehicleWidth; /* VehicleWidth (1..62) */
    LongitudinalAcceleration longitudinalAcceleration;
    Curvature curvature;
    CurvatureCalculationMode curvatureCalculationMode;
    YawRate yawRate;
    BOOL accelerationControl_option;
    BitString accelerationControl; /* AccelerationControl (7..7) */
    BOOL lanePosition_option;
    int32_t lanePosition; /* LanePosition (-1..14) */
    BOOL steeringWheelAngle_option;
    SteeringWheelAngle steeringWheelAngle;
    BOOL lateralAcceleration_option;
    LateralAcceleration lateralAcceleration;
    BOOL verticalAcceleration_option;
    VerticalAcceleration verticalAcceleration;
    BOOL performanceClass_option;
    int32_t performanceClass; /* PerformanceClass (0..7) */
    BOOL cenDsrcTollingZone_option;
    CenDsrcTollingZone cenDsrcTollingZone;
} BasicVehicleContainerHighFrequency;

typedef struct RSUContainerHighFrequency {
    BOOL protectedCommunicationZonesRSU_option;
    ProtectedCommunicationZonesRSU protectedCommunicationZonesRSU;
} RSUContainerHighFrequency;

typedef enum {
    HighFrequencyContainer_basicVehicleContainerHighFrequency,
    HighFrequencyContainer_rsuContainerHighFrequency,
} HighFrequencyContainer_choice;

typedef struct HighFrequencyContainer {
    HighFrequencyContainer_choice choice;
    union {
        BasicVehicleContainerHighFrequency basicVehicleContainerHighFrequency;
        RSUContainerHighFrequency rsuContainerHighFrequency;
    } u;
} HighFrequencyContainer;

typedef struct BasicVehicleContainerLowFrequency {
    VehicleRole vehicleRole;
    BitString exteriorLights; /* ExteriorLights (8..8) */
    PathHistory_ITS pathHistory;
} BasicVehicleContainerLowFrequency;

typedef enum {
    LowFrequencyContainer_basicVehicleContainerLowFrequency,
} LowFrequencyContainer_choice;

typedef struct LowFrequencyContainer {
    LowFrequencyContainer_choice choice;
    union {
        BasicVehicleContainerLowFrequency basicVehicleContainerLowFrequency;
    } u;
} LowFrequencyContainer;

typedef struct PublicTransportContainer {
    BOOL embarkationStatus; /* EmbarkationStatus */
    BOOL ptActivation_option;
    PtActivation ptActivation;
} PublicTransportContainer;

typedef struct SpecialTransportContainer {
    BitString specialTransportType; /* SpecialTransportType (4..4) */
    BitString lightBarSirenInUse; /* LightBarSirenInUse (2..2) */
} SpecialTransportContainer;

typedef struct DangerousGoodsContainer {
    DangerousGoodsBasic dangerousGoodsBasic;
} DangerousGoodsContainer;

typedef struct RoadWorksContainerBasic {
    BOOL roadworksSubCauseCode_option;
    int32_t roadworksSubCauseCode; /* RoadworksSubCauseCode (0..255) */
    BitString lightBarSirenInUse; /* LightBarSirenInUse (2..2) */
    BOOL closedLanes_option;
    ClosedLanes closedLanes;
} RoadWorksContainerBasic;

typedef struct RescueContainer {
    BitString lightBarSirenInUse; /* LightBarSirenInUse (2..2) */
} RescueContainer;

typedef struct EmergencyContainer {
    BitString lightBarSirenInUse; /* LightBarSirenInUse (2..2) */
    BOOL incidentIndication_option;
    CauseCode incidentIndication;
    BOOL emergencyPriority_option;
    BitString emergencyPriority; /* EmergencyPriority (2..2) */
} EmergencyContainer;

typedef struct SafetyCarContainer {
    BitString lightBarSirenInUse; /* LightBarSirenInUse (2..2) */
    BOOL incidentIndication_option;
    CauseCode incidentIndication;
    BOOL trafficRule_option;
    TrafficRule trafficRule;
    BOOL speedLimit_option;
    int32_t speedLimit; /* SpeedLimit (1..255) */
} SafetyCarContainer;

typedef enum {
    SpecialVehicleContainer_publicTransportContainer,
    SpecialVehicleContainer_specialTransportContainer,
    SpecialVehicleContainer_dangerousGoodsContainer,
    SpecialVehicleContainer_roadWorksContainerBasic,
    SpecialVehicleContainer_rescueContainer,
    SpecialVehicleContainer_emergencyContainer,
    SpecialVehicleContainer_safetyCarContainer,
} SpecialVehicleContainer_choice;

typedef struct SpecialVehicleContainer {
    SpecialVehicleContainer_choice choice;
    union {
        PublicTransportContainer publicTransportContainer;
        SpecialTransportContainer specialTransportContainer;
        DangerousGoodsContainer dangerousGoodsContainer;
        RoadWorksContainerBasic roadWorksContainerBasic;
        RescueContainer rescueContainer;
        EmergencyContainer emergencyContainer;
        SafetyCarContainer safetyCarContainer;
    } u;
} SpecialVehicleContainer;

typedef struct CamParameters {
    BasicContainer basicContainer;
    HighFrequencyContainer highFrequencyContainer;
    BOOL lowFrequencyContainer_option;
    LowFrequencyContainer lowFrequencyContainer;
    BOOL specialVehicleContainer_option;
    SpecialVehicleContainer specialVehicleContainer;
} CamParameters;

typedef struct CoopAwareness {
    int32_t generationDeltaTime; /* GenerationDeltaTime (0..65535) */
    CamParameters camParameters;
} CoopAwareness;

typedef struct CAM {
    ItsPduHeader header;
    CoopAwareness cam;
} CAM;

#ifdef __cplusplus
}
#endif

#endif /* __ITSMSG_CAM_H__ */
