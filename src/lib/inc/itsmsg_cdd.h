/* Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef __ITSMSG_CDD_H__
#define __ITSMSG_CDD_H__

#include "asn1defs_if.h"
#include "limits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ItsPduHeader {
    int32_t protocolVersion;
    int32_t messageID;
    uint32_t stationID; /* StationID (0..4294967295) */
} ItsPduHeader;

enum { /* Pre-defined values of Latitude */
       Latitude_oneMicrodegreeNorth = 10,
       Latitude_oneMicrodegreeSouth = -10,
       Latitude_unavailable = 900000001,
};

enum { /* Pre-defined values of Longitude */
       Longitude_oneMicrodegreeEast = 10,
       Longitude_oneMicrodegreeWest = -10,
       Longitude_unavailable = 1800000001,
};

enum { /* Pre-defined values of SemiAxisLength */
       SemiAxisLength_oneCentimeter = 1,
       SemiAxisLength_outOfRange = 4094,
       SemiAxisLength_unavailable = 4095,
};

enum { /* Pre-defined values of HeadingValue */
       HeadingValue_wgs84North = 0,
       HeadingValue_wgs84East = 900,
       HeadingValue_wgs84South = 1800,
       HeadingValue_wgs84West = 2700,
       HeadingValue_unavailable = 3601,
};

typedef struct PosConfidenceEllipse {
    int32_t semiMajorConfidence; /* SemiAxisLength (0..4095) */
    int32_t semiMinorConfidence; /* SemiAxisLength (0..4095) */
    int32_t semiMajorOrientation; /* HeadingValue (0..3601) */
} PosConfidenceEllipse;

enum { /* Pre-defined values of AltitudeValue */
       AltitudeValue_referenceEllipsoidSurface = 0,
       AltitudeValue_oneCentimeter = 1,
       AltitudeValue_unavailable = 800001,
};

typedef enum AltitudeConfidence {
    AltitudeConfidence_alt_000_01,
    AltitudeConfidence_alt_000_02,
    AltitudeConfidence_alt_000_05,
    AltitudeConfidence_alt_000_10,
    AltitudeConfidence_alt_000_20,
    AltitudeConfidence_alt_000_50,
    AltitudeConfidence_alt_001_00,
    AltitudeConfidence_alt_002_00,
    AltitudeConfidence_alt_005_00,
    AltitudeConfidence_alt_010_00,
    AltitudeConfidence_alt_020_00,
    AltitudeConfidence_alt_050_00,
    AltitudeConfidence_alt_100_00,
    AltitudeConfidence_alt_200_00,
    AltitudeConfidence_outOfRange,
    AltitudeConfidence_unavailable,
    AltitudeConfidence_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} AltitudeConfidence;

typedef struct Altitude {
    int32_t altitudeValue; /* AltitudeValue (-100000..800001) */
    AltitudeConfidence altitudeConfidence;
} Altitude;

typedef struct ReferencePosition {
    int32_t latitude; /* Latitude (-900000000..900000001) */
    int32_t longitude; /* Longitude (-1800000000..1800000001) */
    PosConfidenceEllipse positionConfidenceEllipse;
    Altitude altitude;
} ReferencePosition;

enum { /* Pre-defined values of DeltaLatitude */
       DeltaLatitude_oneMicrodegreeNorth = 10,
       DeltaLatitude_oneMicrodegreeSouth = -10,
       DeltaLatitude_unavailable = 131072,
};

enum { /* Pre-defined values of DeltaLongitude */
       DeltaLongitude_oneMicrodegreeEast = 10,
       DeltaLongitude_oneMicrodegreeWest = -10,
       DeltaLongitude_unavailable = 131072,
};

enum { /* Pre-defined values of DeltaAltitude */
       DeltaAltitude_oneCentimeterUp = 1,
       DeltaAltitude_oneCentimeterDown = -1,
       DeltaAltitude_unavailable = 12800,
};

typedef struct DeltaReferencePosition {
    int32_t deltaLatitude; /* DeltaLatitude (-131071..131072) */
    int32_t deltaLongitude; /* DeltaLongitude (-131071..131072) */
    int32_t deltaAltitude; /* DeltaAltitude (-12700..12800) */
} DeltaReferencePosition;

enum { /* Pre-defined values of PathDeltaTime */
       PathDeltaTime_tenMilliSecondsInPast = 1,
};

typedef struct PathPoint {
    DeltaReferencePosition pathPosition;
    BOOL pathDeltaTime_option;
    int32_t pathDeltaTime; /* PathDeltaTime (1..65535) */
} PathPoint;

enum { /* Pre-defined values of PtActivationType */
       PtActivationType_undefinedCodingType = 0,
       PtActivationType_r09_16CodingType = 1,
       PtActivationType_vdv_50149CodingType = 2,
};

typedef struct PtActivation {
    int32_t ptActivationType; /* PtActivationType (0..255) */
    OctetString ptActivationData; /* PtActivationData (1..20) */
} PtActivation;

#define AccelerationControl_MAX_BITS 7
enum { /* Bit position of BitString: AccelerationControl */
       AccelerationControl_brakePedalEngaged = 0,
       AccelerationControl_gasPedalEngaged = 1,
       AccelerationControl_emergencyBrakeEngaged = 2,
       AccelerationControl_collisionWarningEngaged = 3,
       AccelerationControl_accEngaged = 4,
       AccelerationControl_cruiseControlEngaged = 5,
       AccelerationControl_speedLimiterEngaged = 6,
};

enum { /* Pre-defined values of CauseCodeType */
       CauseCodeType_reserved = 0,
       CauseCodeType_trafficCondition = 1,
       CauseCodeType_accident = 2,
       CauseCodeType_roadworks = 3,
       CauseCodeType_adverseWeatherCondition_Adhesion = 6,
       CauseCodeType_hazardousLocation_SurfaceCondition = 9,
       CauseCodeType_hazardousLocation_ObstacleOnTheRoad = 10,
       CauseCodeType_hazardousLocation_AnimalOnTheRoad = 11,
       CauseCodeType_humanPresenceOnTheRoad = 12,
       CauseCodeType_wrongWayDriving = 14,
       CauseCodeType_rescueAndRecoveryWorkInProgress = 15,
       CauseCodeType_adverseWeatherCondition_ExtremeWeatherCondition = 17,
       CauseCodeType_adverseWeatherCondition_Visibility = 18,
       CauseCodeType_adverseWeatherCondition_Precipitation = 19,
       CauseCodeType_slowVehicle = 26,
       CauseCodeType_dangerousEndOfQueue = 27,
       CauseCodeType_vehicleBreakdown = 91,
       CauseCodeType_postCrash = 92,
       CauseCodeType_humanProblem = 93,
       CauseCodeType_stationaryVehicle = 94,
       CauseCodeType_emergencyVehicleApproaching = 95,
       CauseCodeType_hazardousLocation_DangerousCurve = 96,
       CauseCodeType_collisionRisk = 97,
       CauseCodeType_signalViolation = 98,
       CauseCodeType_dangerousSituation = 99,
       CauseCodeType_commercialVehicleSituation = 100,
};

typedef struct CauseCode {
    int32_t causeCode; /* CauseCodeType (0..255) */
    int32_t subCauseCode; /* SubCauseCodeType (0..255) */
} CauseCode;

enum { /* Pre-defined values of TrafficConditionSubCauseCode */
       TrafficConditionSubCauseCode_unavailable = 0,
       TrafficConditionSubCauseCode_increasedVolumeOfTraffic = 1,
       TrafficConditionSubCauseCode_trafficJamSlowlyIncreasing = 2,
       TrafficConditionSubCauseCode_trafficJamIncreasing = 3,
       TrafficConditionSubCauseCode_trafficJamStronglyIncreasing = 4,
       TrafficConditionSubCauseCode_trafficStationary = 5,
       TrafficConditionSubCauseCode_trafficJamSlightlyDecreasing = 6,
       TrafficConditionSubCauseCode_trafficJamDecreasing = 7,
       TrafficConditionSubCauseCode_trafficJamStronglyDecreasing = 8,
       TrafficConditionSubCauseCode_trafficRedLight = 9,
       TrafficConditionSubCauseCode_trafficGreenLight =10, // TODO Remove
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of TrafficCondition */
       TrafficCondition_unavailable = 1 << 4 | 0,
       TrafficCondition_increasedVolumeOfTraffic = 1 << 4 | 1,
       TrafficCondition_trafficJamSlowlyIncreasing = 1 << 4 | 2,
       TrafficCondition_trafficJamIncreasing = 1 << 4 | 3,
       TrafficCondition_trafficJamStronglyIncreasing = 1 << 4 | 4,
       TrafficCondition_trafficStationary = 1 << 4 | 5,
       TrafficCondition_trafficJamSlightlyDecreasing = 1 << 4 | 6,
       TrafficCondition_trafficJamDecreasing = 1 << 4 | 7,
       TrafficCondition_trafficJamStronglyDecreasing = 1 << 4 | 8,
};

enum { /* Pre-defined values of AccidentSubCauseCode */
       AccidentSubCauseCode_unavailable = 0,
       AccidentSubCauseCode_multiVehicleAccident = 1,
       AccidentSubCauseCode_heavyAccident = 2,
       AccidentSubCauseCode_accidentInvolvingLorry = 3,
       AccidentSubCauseCode_accidentInvolvingBus = 4,
       AccidentSubCauseCode_accidentInvolvingHazardousMaterials = 5,
       AccidentSubCauseCode_accidentOnOppositeLane = 6,
       AccidentSubCauseCode_unsecuredAccident = 7,
       AccidentSubCauseCode_assistanceRequested = 8,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of Accident */
       Accident_unavailable = 2 << 4 | 0,
       Accident_multiVehicleAccident = 2 << 4 | 1,
       Accident_heavyAccident = 2 << 4 | 2,
       Accident_accidentInvolvingLorry = 2 << 4 | 3,
       Accident_accidentInvolvingBus = 2 << 4 | 4,
       Accident_accidentInvolvingHazardousMaterials = 2 << 4 | 5,
       Accident_accidentOnOppositeLane = 2 << 4 | 6,
       Accident_unsecuredAccident = 2 << 4 | 7,
       Accident_assistanceRequested = 2 << 4 | 8,
};

enum { /* Pre-defined values of RoadworksSubCauseCode */
       RoadworksSubCauseCode_unavailable = 0,
       RoadworksSubCauseCode_majorRoadworks = 1,
       RoadworksSubCauseCode_roadMarkingWork = 2,
       RoadworksSubCauseCode_slowMovingRoadMaintenance = 3,
       RoadworksSubCauseCode_shortTermStationaryRoadworks = 4,
       RoadworksSubCauseCode_streetCleaning = 5,
       RoadworksSubCauseCode_winterService = 6,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of Roadworks */
       Roadworks_unavailable = 3 << 4 | 0,
       Roadworks_majorRoadworks = 3 << 4 | 1,
       Roadworks_roadMarkingWork = 3 << 4 | 2,
       Roadworks_slowMovingRoadMaintenance = 3 << 4 | 3,
       Roadworks_shortTermStationaryRoadworks = 3 << 4 | 4,
       Roadworks_streetCleaning = 3 << 4 | 5,
       Roadworks_winterService = 3 << 4 | 6,
};

enum { /* Pre-defined values of HumanPresenceOnTheRoadSubCauseCode */
       HumanPresenceOnTheRoadSubCauseCode_unavailable = 0,
       HumanPresenceOnTheRoadSubCauseCode_childrenOnRoadway = 1,
       HumanPresenceOnTheRoadSubCauseCode_cyclistOnRoadway = 2,
       HumanPresenceOnTheRoadSubCauseCode_motorcyclistOnRoadway = 3,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of HumanPresenceOnTheRoad */
       HumanPresenceOnTheRoad_unavailable = 12 << 4 | 0,
       HumanPresenceOnTheRoad_childrenOnRoadway = 12 << 4 | 1,
       HumanPresenceOnTheRoad_cyclistOnRoadway = 12 << 4 | 2,
       HumanPresenceOnTheRoad_motorcyclistOnRoadway = 12 << 4 | 3,
};

enum { /* Pre-defined values of WrongWayDrivingSubCauseCode */
       WrongWayDrivingSubCauseCode_unavailable = 0,
       WrongWayDrivingSubCauseCode_wrongLane = 1,
       WrongWayDrivingSubCauseCode_wrongDirection = 2,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of WrongWayDriving */
       WrongWayDriving_unavailable = 14 << 4 | 0,
       WrongWayDriving_wrongLane = 14 << 4 | 1,
       WrongWayDriving_wrongDirection = 14 << 4 | 2,
};

enum { /* Pre-defined values of AdverseWeatherCondition_ExtremeWeatherConditionSubCauseCode */
       AdverseWeatherCondition_ExtremeWeatherConditionSubCauseCode_unavailable = 0,
       AdverseWeatherCondition_ExtremeWeatherConditionSubCauseCode_strongWinds = 1,
       AdverseWeatherCondition_ExtremeWeatherConditionSubCauseCode_damagingHail = 2,
       AdverseWeatherCondition_ExtremeWeatherConditionSubCauseCode_hurricane = 3,
       AdverseWeatherCondition_ExtremeWeatherConditionSubCauseCode_thunderstorm = 4,
       AdverseWeatherCondition_ExtremeWeatherConditionSubCauseCode_tornado = 5,
       AdverseWeatherCondition_ExtremeWeatherConditionSubCauseCode_blizzard = 6,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of AdverseWeatherCondition_ExtremeWeatherCondition */
       AdverseWeatherCondition_ExtremeWeatherCondition_unavailable = 17 << 4 | 0,
       AdverseWeatherCondition_ExtremeWeatherCondition_strongWinds = 17 << 4 | 1,
       AdverseWeatherCondition_ExtremeWeatherCondition_damagingHail = 17 << 4 | 2,
       AdverseWeatherCondition_ExtremeWeatherCondition_hurricane = 17 << 4 | 3,
       AdverseWeatherCondition_ExtremeWeatherCondition_thunderstorm = 17 << 4 | 4,
       AdverseWeatherCondition_ExtremeWeatherCondition_tornado = 17 << 4 | 5,
       AdverseWeatherCondition_ExtremeWeatherCondition_blizzard = 17 << 4 | 6,
};

enum { /* Pre-defined values of AdverseWeatherCondition_AdhesionSubCauseCode */
       AdverseWeatherCondition_AdhesionSubCauseCode_unavailable = 0,
       AdverseWeatherCondition_AdhesionSubCauseCode_heavyFrostOnRoad = 1,
       AdverseWeatherCondition_AdhesionSubCauseCode_fuelOnRoad = 2,
       AdverseWeatherCondition_AdhesionSubCauseCode_mudOnRoad = 3,
       AdverseWeatherCondition_AdhesionSubCauseCode_snowOnRoad = 4,
       AdverseWeatherCondition_AdhesionSubCauseCode_iceOnRoad = 5,
       AdverseWeatherCondition_AdhesionSubCauseCode_blackIceOnRoad = 6,
       AdverseWeatherCondition_AdhesionSubCauseCode_oilOnRoad = 7,
       AdverseWeatherCondition_AdhesionSubCauseCode_looseChippings = 8,
       AdverseWeatherCondition_AdhesionSubCauseCode_instantBlackIce = 9,
       AdverseWeatherCondition_AdhesionSubCauseCode_roadsSalted = 10,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of AdverseWeatherCondition_Adhesion */
       AdverseWeatherCondition_Adhesion_unavailable = 6 << 4 | 0,
       AdverseWeatherCondition_Adhesion_heavyFrostOnRoad = 6 << 4 | 1,
       AdverseWeatherCondition_Adhesion_fuelOnRoad = 6 << 4 | 2,
       AdverseWeatherCondition_Adhesion_mudOnRoad = 6 << 4 | 3,
       AdverseWeatherCondition_Adhesion_snowOnRoad = 6 << 4 | 4,
       AdverseWeatherCondition_Adhesion_iceOnRoad = 6 << 4 | 5,
       AdverseWeatherCondition_Adhesion_blackIceOnRoad = 6 << 4 | 6,
       AdverseWeatherCondition_Adhesion_oilOnRoad = 6 << 4 | 7,
       AdverseWeatherCondition_Adhesion_looseChippings = 6 << 4 | 8,
       AdverseWeatherCondition_Adhesion_instantBlackIce = 6 << 4 | 9,
       AdverseWeatherCondition_Adhesion_roadsSalted = 6 << 4 | 10,
};

enum { /* Pre-defined values of AdverseWeatherCondition_VisibilitySubCauseCode */
       AdverseWeatherCondition_VisibilitySubCauseCode_unavailable = 0,
       AdverseWeatherCondition_VisibilitySubCauseCode_fog = 1,
       AdverseWeatherCondition_VisibilitySubCauseCode_smoke = 2,
       AdverseWeatherCondition_VisibilitySubCauseCode_heavySnowfall = 3,
       AdverseWeatherCondition_VisibilitySubCauseCode_heavyRain = 4,
       AdverseWeatherCondition_VisibilitySubCauseCode_heavyHail = 5,
       AdverseWeatherCondition_VisibilitySubCauseCode_lowSunGlare = 6,
       AdverseWeatherCondition_VisibilitySubCauseCode_sandstorms = 7,
       AdverseWeatherCondition_VisibilitySubCauseCode_swarmsOfInsects = 8,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of AdverseWeatherCondition_Visibility */
       AdverseWeatherCondition_Visibility_unavailable = 18 << 4 | 0,
       AdverseWeatherCondition_Visibility_fog = 18 << 4 | 1,
       AdverseWeatherCondition_Visibility_smoke = 18 << 4 | 2,
       AdverseWeatherCondition_Visibility_heavySnowfall = 18 << 4 | 3,
       AdverseWeatherCondition_Visibility_heavyRain = 18 << 4 | 4,
       AdverseWeatherCondition_Visibility_heavyHail = 18 << 4 | 5,
       AdverseWeatherCondition_Visibility_lowSunGlare = 18 << 4 | 6,
       AdverseWeatherCondition_Visibility_sandstorms = 18 << 4 | 7,
       AdverseWeatherCondition_Visibility_swarmsOfInsects = 18 << 4 | 8,
};

enum { /* Pre-defined values of AdverseWeatherCondition_PrecipitationSubCauseCode */
       AdverseWeatherCondition_PrecipitationSubCauseCode_unavailable = 0,
       AdverseWeatherCondition_PrecipitationSubCauseCode_heavyRain = 1,
       AdverseWeatherCondition_PrecipitationSubCauseCode_heavySnowfall = 2,
       AdverseWeatherCondition_PrecipitationSubCauseCode_softHail = 3,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of AdverseWeatherCondition_Precipitation */
       AdverseWeatherCondition_Precipitation_unavailable = 19 << 4 | 0,
       AdverseWeatherCondition_Precipitation_heavyRain = 19 << 4 | 1,
       AdverseWeatherCondition_Precipitation_heavySnowfall = 19 << 4 | 2,
       AdverseWeatherCondition_Precipitation_softHail = 19 << 4 | 3,
};

enum { /* Pre-defined values of SlowVehicleSubCauseCode */
       SlowVehicleSubCauseCode_unavailable = 0,
       SlowVehicleSubCauseCode_maintenanceVehicle = 1,
       SlowVehicleSubCauseCode_vehiclesSlowingToLookAtAccident = 2,
       SlowVehicleSubCauseCode_abnormalLoad = 3,
       SlowVehicleSubCauseCode_abnormalWideLoad = 4,
       SlowVehicleSubCauseCode_convoy = 5,
       SlowVehicleSubCauseCode_snowplough = 6,
       SlowVehicleSubCauseCode_deicing = 7,
       SlowVehicleSubCauseCode_saltingVehicles = 8,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of SlowVehicle */
       SlowVehicle_unavailable = 26 << 4 | 0,
       SlowVehicle_maintenanceVehicle = 26 << 4 | 1,
       SlowVehicle_vehiclesSlowingToLookAtAccident = 26 << 4 | 2,
       SlowVehicle_abnormalLoad = 26 << 4 | 3,
       SlowVehicle_abnormalWideLoad = 26 << 4 | 4,
       SlowVehicle_convoy = 26 << 4 | 5,
       SlowVehicle_snowplough = 26 << 4 | 6,
       SlowVehicle_deicing = 26 << 4 | 7,
       SlowVehicle_saltingVehicles = 26 << 4 | 8,
};

enum { /* Pre-defined values of StationaryVehicleSubCauseCode */
       StationaryVehicleSubCauseCode_unavailable = 0,
       StationaryVehicleSubCauseCode_humanProblem = 1,
       StationaryVehicleSubCauseCode_vehicleBreakdown = 2,
       StationaryVehicleSubCauseCode_postCrash = 3,
       StationaryVehicleSubCauseCode_publicTransportStop = 4,
       StationaryVehicleSubCauseCode_carryingDangerousGoods = 5,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of StationaryVehicle */
       StationaryVehicle_unavailable = 94 << 4 | 0,
       StationaryVehicle_humanProblem = 94 << 4 | 1,
       StationaryVehicle_vehicleBreakdown = 94 << 4 | 2,
       StationaryVehicle_postCrash = 94 << 4 | 3,
       StationaryVehicle_publicTransportStop = 94 << 4 | 4,
       StationaryVehicle_carryingDangerousGoods = 94 << 4 | 5,
};

enum { /* Pre-defined values of HumanProblemSubCauseCode */
       HumanProblemSubCauseCode_unavailable = 0,
       HumanProblemSubCauseCode_glycemiaProblem = 1,
       HumanProblemSubCauseCode_heartProblem = 2,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of HumanProblem */
       HumanProblem_unavailable = 93 << 4 | 0,
       HumanProblem_glycemiaProblem = 93 << 4 | 1,
       HumanProblem_heartProblem = 93 << 4 | 2,
};

enum { /* Pre-defined values of EmergencyVehicleApproachingSubCauseCode */
       EmergencyVehicleApproachingSubCauseCode_unavailable = 0,
       EmergencyVehicleApproachingSubCauseCode_emergencyVehicleApproaching = 1,
       EmergencyVehicleApproachingSubCauseCode_prioritizedVehicleApproaching = 2,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of EmergencyVehicleApproaching */
       EmergencyVehicleApproaching_unavailable = 95 << 4 | 0,
       EmergencyVehicleApproaching_emergencyVehicleApproaching = 95 << 4 | 1,
       EmergencyVehicleApproaching_prioritizedVehicleApproaching = 95 << 4 | 2,
};

enum { /* Pre-defined values of HazardousLocation_DangerousCurveSubCauseCode */
       HazardousLocation_DangerousCurveSubCauseCode_unavailable = 0,
       HazardousLocation_DangerousCurveSubCauseCode_dangerousLeftTurnCurve = 1,
       HazardousLocation_DangerousCurveSubCauseCode_dangerousRightTurnCurve = 2,
       HazardousLocation_DangerousCurveSubCauseCode_multipleCurvesStartingWithUnknownTurningDirection = 3,
       HazardousLocation_DangerousCurveSubCauseCode_multipleCurvesStartingWithLeftTurn = 4,
       HazardousLocation_DangerousCurveSubCauseCode_multipleCurvesStartingWithRightTurn = 5,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of HazardousLocation_DangerousCurve */
       HazardousLocation_DangerousCurve_unavailable = 96 << 4 | 0,
       HazardousLocation_DangerousCurve_dangerousLeftTurnCurve = 96 << 4 | 1,
       HazardousLocation_DangerousCurve_dangerousRightTurnCurve = 96 << 4 | 2,
       HazardousLocation_DangerousCurve_multipleCurvesStartingWithUnknownTurningDirection = 96 << 4 | 3,
       HazardousLocation_DangerousCurve_multipleCurvesStartingWithLeftTurn = 96 << 4 | 4,
       HazardousLocation_DangerousCurve_multipleCurvesStartingWithRightTurn = 96 << 4 | 5,
};

enum { /* Pre-defined values of HazardousLocation_SurfaceConditionSubCauseCode */
       HazardousLocation_SurfaceConditionSubCauseCode_unavailable = 0,
       HazardousLocation_SurfaceConditionSubCauseCode_rockfalls = 1,
       HazardousLocation_SurfaceConditionSubCauseCode_earthquakeDamage = 2,
       HazardousLocation_SurfaceConditionSubCauseCode_sewerCollapse = 3,
       HazardousLocation_SurfaceConditionSubCauseCode_subsidence = 4,
       HazardousLocation_SurfaceConditionSubCauseCode_snowDrifts = 5,
       HazardousLocation_SurfaceConditionSubCauseCode_stormDamage = 6,
       HazardousLocation_SurfaceConditionSubCauseCode_burstPipe = 7,
       HazardousLocation_SurfaceConditionSubCauseCode_volcanoEruption = 8,
       HazardousLocation_SurfaceConditionSubCauseCode_fallingIce = 9,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of HazardousLocation_SurfaceCondition */
       HazardousLocation_SurfaceCondition_unavailable = 9 << 4 | 0,
       HazardousLocation_SurfaceCondition_rockfalls = 9 << 4 | 1,
       HazardousLocation_SurfaceCondition_earthquakeDamage = 9 << 4 | 2,
       HazardousLocation_SurfaceCondition_sewerCollapse = 9 << 4 | 3,
       HazardousLocation_SurfaceCondition_subsidence = 9 << 4 | 4,
       HazardousLocation_SurfaceCondition_snowDrifts = 9 << 4 | 5,
       HazardousLocation_SurfaceCondition_stormDamage = 9 << 4 | 6,
       HazardousLocation_SurfaceCondition_burstPipe = 9 << 4 | 7,
       HazardousLocation_SurfaceCondition_volcanoEruption = 9 << 4 | 8,
       HazardousLocation_SurfaceCondition_fallingIce = 9 << 4 | 9,
};

enum { /* Pre-defined values of HazardousLocation_ObstacleOnTheRoadSubCauseCode */
       HazardousLocation_ObstacleOnTheRoadSubCauseCode_unavailable = 0,
       HazardousLocation_ObstacleOnTheRoadSubCauseCode_shedLoad = 1,
       HazardousLocation_ObstacleOnTheRoadSubCauseCode_partsOfVehicles = 2,
       HazardousLocation_ObstacleOnTheRoadSubCauseCode_partsOfTyres = 3,
       HazardousLocation_ObstacleOnTheRoadSubCauseCode_bigObjects = 4,
       HazardousLocation_ObstacleOnTheRoadSubCauseCode_fallenTrees = 5,
       HazardousLocation_ObstacleOnTheRoadSubCauseCode_hubCaps = 6,
       HazardousLocation_ObstacleOnTheRoadSubCauseCode_waitingVehicles = 7,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of HazardousLocation_ObstacleOnTheRoad */
       HazardousLocation_ObstacleOnTheRoad_unavailable = 10 << 4 | 0,
       HazardousLocation_ObstacleOnTheRoad_shedLoad = 10 << 4 | 1,
       HazardousLocation_ObstacleOnTheRoad_partsOfVehicles = 10 << 4 | 2,
       HazardousLocation_ObstacleOnTheRoad_partsOfTyres = 10 << 4 | 3,
       HazardousLocation_ObstacleOnTheRoad_bigObjects = 10 << 4 | 4,
       HazardousLocation_ObstacleOnTheRoad_fallenTrees = 10 << 4 | 5,
       HazardousLocation_ObstacleOnTheRoad_hubCaps = 10 << 4 | 6,
       HazardousLocation_ObstacleOnTheRoad_waitingVehicles = 10 << 4 | 7,
};

enum { /* Pre-defined values of HazardousLocation_AnimalOnTheRoadSubCauseCode */
       HazardousLocation_AnimalOnTheRoadSubCauseCode_unavailable = 0,
       HazardousLocation_AnimalOnTheRoadSubCauseCode_wildAnimals = 1,
       HazardousLocation_AnimalOnTheRoadSubCauseCode_herdOfAnimals = 2,
       HazardousLocation_AnimalOnTheRoadSubCauseCode_smallAnimals = 3,
       HazardousLocation_AnimalOnTheRoadSubCauseCode_largeAnimals = 4,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of HazardousLocation_AnimalOnTheRoad */
       HazardousLocation_AnimalOnTheRoad_unavailable = 11 << 4 | 0,
       HazardousLocation_AnimalOnTheRoad_wildAnimals = 11 << 4 | 1,
       HazardousLocation_AnimalOnTheRoad_herdOfAnimals = 11 << 4 | 2,
       HazardousLocation_AnimalOnTheRoad_smallAnimals = 11 << 4 | 3,
       HazardousLocation_AnimalOnTheRoad_largeAnimals = 11 << 4 | 4,
};

enum { /* Pre-defined values of CollisionRiskSubCauseCode */
       CollisionRiskSubCauseCode_unavailable = 0,
       CollisionRiskSubCauseCode_longitudinalCollisionRisk = 1,
       CollisionRiskSubCauseCode_crossingCollisionRisk = 2,
       CollisionRiskSubCauseCode_lateralCollisionRisk = 3,
       CollisionRiskSubCauseCode_vulnerableRoadUser = 4,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of CollisionRisk */
       CollisionRisk_unavailable = 97 << 4 | 0,
       CollisionRisk_longitudinalCollisionRisk = 97 << 4 | 1,
       CollisionRisk_crossingCollisionRisk = 97 << 4 | 2,
       CollisionRisk_lateralCollisionRisk = 97 << 4 | 3,
       CollisionRisk_vulnerableRoadUser = 97 << 4 | 4,
};

enum { /* Pre-defined values of SignalViolationSubCauseCode */
       SignalViolationSubCauseCode_unavailable = 0,
       SignalViolationSubCauseCode_stopSignViolation = 1,
       SignalViolationSubCauseCode_trafficLightViolation = 2,
       SignalViolationSubCauseCode_turningRegulationViolation = 3,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of SignalViolation */
       SignalViolation_unavailable = 98 << 4 | 0,
       SignalViolation_stopSignViolation = 98 << 4 | 1,
       SignalViolation_trafficLightViolation = 98 << 4 | 2,
       SignalViolation_turningRegulationViolation = 98 << 4 | 3,
};

enum { /* Pre-defined values of RescueAndRecoveryWorkInProgressSubCauseCode */
       RescueAndRecoveryWorkInProgressSubCauseCode_unavailable = 0,
       RescueAndRecoveryWorkInProgressSubCauseCode_emergencyVehicles = 1,
       RescueAndRecoveryWorkInProgressSubCauseCode_rescueHelicopterLanding = 2,
       RescueAndRecoveryWorkInProgressSubCauseCode_policeActivityOngoing = 3,
       RescueAndRecoveryWorkInProgressSubCauseCode_medicalEmergencyOngoing = 4,
       RescueAndRecoveryWorkInProgressSubCauseCode_childAbductionInProgress = 5,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of RescueAndRecoveryWorkInProgress */
       RescueAndRecoveryWorkInProgress_unavailable = 15 << 4 | 0,
       RescueAndRecoveryWorkInProgress_emergencyVehicles = 15 << 4 | 1,
       RescueAndRecoveryWorkInProgress_rescueHelicopterLanding = 15 << 4 | 2,
       RescueAndRecoveryWorkInProgress_policeActivityOngoing = 15 << 4 | 3,
       RescueAndRecoveryWorkInProgress_medicalEmergencyOngoing = 15 << 4 | 4,
       RescueAndRecoveryWorkInProgress_childAbductionInProgress = 15 << 4 | 5,
};

enum { /* Pre-defined values of DangerousEndOfQueueSubCauseCode */
       DangerousEndOfQueueSubCauseCode_unavailable = 0,
       DangerousEndOfQueueSubCauseCode_suddenEndOfQueue = 1,
       DangerousEndOfQueueSubCauseCode_queueOverHill = 2,
       DangerousEndOfQueueSubCauseCode_queueAroundBend = 3,
       DangerousEndOfQueueSubCauseCode_queueInTunnel = 4,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of DangerousEndOfQueue */
       DangerousEndOfQueue_unavailable = 27 << 4 | 0,
       DangerousEndOfQueue_suddenEndOfQueue = 27 << 4 | 1,
       DangerousEndOfQueue_queueOverHill = 27 << 4 | 2,
       DangerousEndOfQueue_queueAroundBend = 27 << 4 | 3,
       DangerousEndOfQueue_queueInTunnel = 27 << 4 | 4,
};

enum { /* Pre-defined values of DangerousSituationSubCauseCode */
       DangerousSituationSubCauseCode_unavailable = 0,
       DangerousSituationSubCauseCode_emergencyElectronicBrakeEngaged = 1,
       DangerousSituationSubCauseCode_preCrashSystemEngaged = 2,
       DangerousSituationSubCauseCode_espEngaged = 3,
       DangerousSituationSubCauseCode_absEngaged = 4,
       DangerousSituationSubCauseCode_aebEngaged = 5,
       DangerousSituationSubCauseCode_brakeWarningEngaged = 6,
       DangerousSituationSubCauseCode_collisionRiskWarningEngaged = 7,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of DangerousSituation */
       DangerousSituation_unavailable = 99 << 4 | 0,
       DangerousSituation_emergencyElectronicBrakeEngaged = 99 << 4 | 1,
       DangerousSituation_preCrashSystemEngaged = 99 << 4 | 2,
       DangerousSituation_espEngaged = 99 << 4 | 3,
       DangerousSituation_absEngaged = 99 << 4 | 4,
       DangerousSituation_aebEngaged = 99 << 4 | 5,
       DangerousSituation_brakeWarningEngaged = 99 << 4 | 6,
       DangerousSituation_collisionRiskWarningEngaged = 99 << 4 | 7,
};

enum { /* Pre-defined values of VehicleBreakdownSubCauseCode */
       VehicleBreakdownSubCauseCode_unavailable = 0,
       VehicleBreakdownSubCauseCode_lackOfFuel = 1,
       VehicleBreakdownSubCauseCode_lackOfBatteryPower = 2,
       VehicleBreakdownSubCauseCode_engineProblem = 3,
       VehicleBreakdownSubCauseCode_transmissionProblem = 4,
       VehicleBreakdownSubCauseCode_engineCoolingProblem = 5,
       VehicleBreakdownSubCauseCode_brakingSystemProblem = 6,
       VehicleBreakdownSubCauseCode_steeringProblem = 7,
       VehicleBreakdownSubCauseCode_tyrePuncture = 8,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of VehicleBreakdown */
       VehicleBreakdown_unavailable = 91 << 4 | 0,
       VehicleBreakdown_lackOfFuel = 91 << 4 | 1,
       VehicleBreakdown_lackOfBatteryPower = 91 << 4 | 2,
       VehicleBreakdown_engineProblem = 91 << 4 | 3,
       VehicleBreakdown_transmissionProblem = 91 << 4 | 4,
       VehicleBreakdown_engineCoolingProblem = 91 << 4 | 5,
       VehicleBreakdown_brakingSystemProblem = 91 << 4 | 6,
       VehicleBreakdown_steeringProblem = 91 << 4 | 7,
       VehicleBreakdown_tyrePuncture = 91 << 4 | 8,
};

enum { /* Pre-defined values of PostCrashSubCauseCode */
       PostCrashSubCauseCode_unavailable = 0,
       PostCrashSubCauseCode_accidentWithoutECallTriggered = 1,
       PostCrashSubCauseCode_accidentWithECallManuallyTriggered = 2,
       PostCrashSubCauseCode_accidentWithECallAutomaticallyTriggered = 3,
       PostCrashSubCauseCode_accidentWithECallTriggeredWithoutAccessToCellularNetwork = 4,
};
enum { /* One enum to represent CauseCodeType and SubCauseCode of PostCrash */
       PostCrash_unavailable = 92 << 4 | 0,
       PostCrash_accidentWithoutECallTriggered = 92 << 4 | 1,
       PostCrash_accidentWithECallManuallyTriggered = 92 << 4 | 2,
       PostCrash_accidentWithECallAutomaticallyTriggered = 92 << 4 | 3,
       PostCrash_accidentWithECallTriggeredWithoutAccessToCellularNetwork = 92 << 4 | 4,
};

enum { /* Pre-defined values of CurvatureValue */
       CurvatureValue_straight = 0,
       CurvatureValue_reciprocalOf1MeterRadiusToRight = -30000,
       CurvatureValue_reciprocalOf1MeterRadiusToLeft = 30000,
       CurvatureValue_unavailable = 30001,
};

typedef enum CurvatureConfidence {
    CurvatureConfidence_onePerMeter_0_00002,
    CurvatureConfidence_onePerMeter_0_0001,
    CurvatureConfidence_onePerMeter_0_0005,
    CurvatureConfidence_onePerMeter_0_002,
    CurvatureConfidence_onePerMeter_0_01,
    CurvatureConfidence_onePerMeter_0_1,
    CurvatureConfidence_outOfRange,
    CurvatureConfidence_unavailable,
    CurvatureConfidence_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} CurvatureConfidence;

typedef struct Curvature {
    int32_t curvatureValue; /* CurvatureValue (-30000..30001) */
    CurvatureConfidence curvatureConfidence;
} Curvature;

typedef enum CurvatureCalculationMode {
    CurvatureCalculationMode_yawRateUsed,
    CurvatureCalculationMode_yawRateNotUsed,
    CurvatureCalculationMode_unavailable,
    CurvatureCalculationMode_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} CurvatureCalculationMode;

enum { /* Pre-defined values of HeadingConfidence */
       HeadingConfidence_equalOrWithinZeroPointOneDegree = 1,
       HeadingConfidence_equalOrWithinOneDegree = 10,
       HeadingConfidence_outOfRange = 126,
       HeadingConfidence_unavailable_ITS = 127,
};

typedef struct Heading {
    int32_t headingValue; /* HeadingValue (0..3601) */
    int32_t headingConfidence; /* HeadingConfidence (1..127) */
} Heading;

enum { /* Pre-defined values of LanePosition */
       LanePosition_offTheRoad = -1,
       LanePosition_hardShoulder = 0,
       LanePosition_outermostDrivingLane = 1,
       LanePosition_secondLaneFromOutside = 2,
};

typedef enum HardShoulderStatus {
    HardShoulderStatus_availableForStopping,
    HardShoulderStatus_closed,
    HardShoulderStatus_availableForDriving,
    HardShoulderStatus_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} HardShoulderStatus;

#define DrivingLaneStatus_MAX_BITS 14
enum { /* Bit position of BitString: DrivingLaneStatus */
       DrivingLaneStatus_outermostLaneClosed = 1,
       DrivingLaneStatus_secondLaneFromOutsideClosed = 2,
};

typedef struct ClosedLanes {
    BOOL hardShoulderStatus_option;
    HardShoulderStatus hardShoulderStatus;
    BitString drivingLaneStatus; /* DrivingLaneStatus (1..14) */
} ClosedLanes;

enum { /* Pre-defined values of PerformanceClass */
       PerformanceClass_unavailable = 0,
       PerformanceClass_performanceClassA = 1,
       PerformanceClass_performanceClassB = 2,
};

enum { /* Pre-defined values of SpeedValue */
       SpeedValue_standstill = 0,
       SpeedValue_oneCentimeterPerSec = 1,
       SpeedValue_unavailable = 16383,
};

enum { /* Pre-defined values of SpeedConfidence */
       SpeedConfidence_equalOrWithinOneCentimeterPerSec = 1,
       SpeedConfidence_equalOrWithinOneMeterPerSec = 100,
       SpeedConfidence_outOfRange = 126,
       SpeedConfidence_unavailable_ITS = 127,
};

enum { /* Pre-defined values of VehicleMass */
       VehicleMass_hundredKg = 1,
       VehicleMass_unavailable = 1024,
};

typedef struct Speed {
    int32_t speedValue; /* SpeedValue (0..16383) */
    int32_t speedConfidence; /* SpeedConfidence (1..127) */
} Speed;

typedef enum DriveDirection {
    DriveDirection_forward,
    DriveDirection_backward,
    DriveDirection_unavailable,
    DriveDirection_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} DriveDirection;

enum { /* Pre-defined values of LongitudinalAccelerationValue */
       LongitudinalAccelerationValue_pointOneMeterPerSecSquaredForward = 1,
       LongitudinalAccelerationValue_pointOneMeterPerSecSquaredBackward = -1,
       LongitudinalAccelerationValue_unavailable = 161,
};

enum { /* Pre-defined values of AccelerationConfidence */
       AccelerationConfidence_pointOneMeterPerSecSquared = 1,
       AccelerationConfidence_outOfRange = 101,
       AccelerationConfidence_unavailable_ITS = 102,
};

typedef struct LongitudinalAcceleration {
    int32_t longitudinalAccelerationValue; /* LongitudinalAccelerationValue (-160..161) */
    int32_t longitudinalAccelerationConfidence; /* AccelerationConfidence (0..102) */
} LongitudinalAcceleration;

enum { /* Pre-defined values of LateralAccelerationValue */
       LateralAccelerationValue_pointOneMeterPerSecSquaredToRight = -1,
       LateralAccelerationValue_pointOneMeterPerSecSquaredToLeft = 1,
       LateralAccelerationValue_unavailable = 161,
};

typedef struct LateralAcceleration {
    int32_t lateralAccelerationValue; /* LateralAccelerationValue (-160..161) */
    int32_t lateralAccelerationConfidence; /* AccelerationConfidence (0..102) */
} LateralAcceleration;

enum { /* Pre-defined values of VerticalAccelerationValue */
       VerticalAccelerationValue_pointOneMeterPerSecSquaredUp = 1,
       VerticalAccelerationValue_pointOneMeterPerSecSquaredDown = -1,
       VerticalAccelerationValue_unavailable = 161,
};

typedef struct VerticalAcceleration {
    int32_t verticalAccelerationValue; /* VerticalAccelerationValue (-160..161) */
    int32_t verticalAccelerationConfidence; /* AccelerationConfidence (0..102) */
} VerticalAcceleration;

enum { /* Pre-defined values of StationType */
       StationType_unknown = 0,
       StationType_pedestrian = 1,
       StationType_cyclist = 2,
       StationType_moped = 3,
       StationType_motorcycle = 4,
       StationType_passengerCar = 5,
       StationType_bus = 6,
       StationType_lightTruck = 7,
       StationType_heavyTruck = 8,
       StationType_trailer = 9,
       StationType_specialVehicles = 10,
       StationType_tram = 11,
       StationType_roadSideUnit = 15,
};

#define ExteriorLights_MAX_BITS_ITS 8
enum { /* Bit position of BitString: ExteriorLights */
       ExteriorLights_lowBeamHeadlightsOn_ITS = 0,
       ExteriorLights_highBeamHeadlightsOn_ITS = 1,
       ExteriorLights_leftTurnSignalOn_ITS = 2,
       ExteriorLights_rightTurnSignalOn_ITS = 3,
       ExteriorLights_daytimeRunningLightsOn_ITS = 4,
       ExteriorLights_reverseLightOn_ITS = 5,
       ExteriorLights_fogLightOn_ITS = 6,
       ExteriorLights_parkingLightsOn_ITS = 7,
};

typedef enum DangerousGoodsBasic {
    DangerousGoodsBasic_explosives1,
    DangerousGoodsBasic_explosives2,
    DangerousGoodsBasic_explosives3,
    DangerousGoodsBasic_explosives4,
    DangerousGoodsBasic_explosives5,
    DangerousGoodsBasic_explosives6,
    DangerousGoodsBasic_flammableGases,
    DangerousGoodsBasic_nonFlammableGases,
    DangerousGoodsBasic_toxicGases,
    DangerousGoodsBasic_flammableLiquids,
    DangerousGoodsBasic_flammableSolids,
    DangerousGoodsBasic_substancesLiableToSpontaneousCombustion,
    DangerousGoodsBasic_substancesEmittingFlammableGasesUponContactWithWater,
    DangerousGoodsBasic_oxidizingSubstances,
    DangerousGoodsBasic_organicPeroxides,
    DangerousGoodsBasic_toxicSubstances,
    DangerousGoodsBasic_infectiousSubstances,
    DangerousGoodsBasic_radioactiveMaterial,
    DangerousGoodsBasic_corrosiveSubstances,
    DangerousGoodsBasic_miscellaneousDangerousSubstances,
    DangerousGoodsBasic_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} DangerousGoodsBasic;

typedef struct DangerousGoodsExtended {
    DangerousGoodsBasic dangerousGoodsType;
    int32_t unNumber;
    BOOL elevatedTemperature;
    BOOL tunnelsRestricted;
    BOOL limitedQuantity;
    BOOL emergencyActionCode_option;
    OctetString emergencyActionCode;
    BOOL phoneNumber_option;
    OctetString phoneNumber;
    BOOL companyName_option;
    OctetString companyName;
} DangerousGoodsExtended;

#define SpecialTransportType_MAX_BITS 4
enum { /* Bit position of BitString: SpecialTransportType */
       SpecialTransportType_heavyLoad = 0,
       SpecialTransportType_excessWidth = 1,
       SpecialTransportType_excessLength = 2,
       SpecialTransportType_excessHeight = 3,
};

#define LightBarSirenInUse_MAX_BITS 2
enum { /* Bit position of BitString: LightBarSirenInUse */
       LightBarSirenInUse_lightBarActivated = 0,
       LightBarSirenInUse_sirenActivated = 1,
};

enum { /* Pre-defined values of HeightLonCarr */
       HeightLonCarr_oneCentimeter = 1,
       HeightLonCarr_unavailable = 100,
};

enum { /* Pre-defined values of PosLonCarr */
       PosLonCarr_oneCentimeter = 1,
       PosLonCarr_unavailable = 127,
};

enum { /* Pre-defined values of PosPillar */
       PosPillar_tenCentimeters = 1,
       PosPillar_unavailable = 30,
};

enum { /* Pre-defined values of PosCentMass */
       PosCentMass_tenCentimeters = 1,
       PosCentMass_unavailable = 63,
};

typedef enum RequestResponseIndication {
    RequestResponseIndication_request,
    RequestResponseIndication_response,
    RequestResponseIndication_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} RequestResponseIndication;

enum { /* Pre-defined values of SpeedLimit */
       SpeedLimit_oneKmPerHour = 1,
};

typedef enum StationarySince {
    StationarySince_lessThan1Minute,
    StationarySince_lessThan2Minutes,
    StationarySince_lessThan15Minutes,
    StationarySince_equalOrGreater15Minutes,
    StationarySince_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} StationarySince;

enum { /* Pre-defined values of Temperature */
       Temperature_equalOrSmallerThanMinus60Deg = -60,
       Temperature_oneDegreeCelsius = 1,
       Temperature_equalOrGreaterThan67Deg = 67,
};

typedef enum TrafficRule {
    TrafficRule_noPassing,
    TrafficRule_noPassingForTrucks,
    TrafficRule_passToRight,
    TrafficRule_passToLeft,
    TrafficRule_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} TrafficRule;

enum { /* Pre-defined values of WheelBaseVehicle */
       WheelBaseVehicle_tenCentimeters = 1,
       WheelBaseVehicle_unavailable = 127,
};

enum { /* Pre-defined values of TurningRadius */
       TurningRadius_point4Meters = 1,
       TurningRadius_unavailable = 255,
};

enum { /* Pre-defined values of PosFrontAx */
       PosFrontAx_tenCentimeters = 1,
       PosFrontAx_unavailable = 20,
};

#define PositionOfOccupants_MAX_BITS 20
enum { /* Bit position of BitString: PositionOfOccupants */
       PositionOfOccupants_row1LeftOccupied = 0,
       PositionOfOccupants_row1RightOccupied = 1,
       PositionOfOccupants_row1MidOccupied = 2,
       PositionOfOccupants_row1NotDetectable = 3,
       PositionOfOccupants_row1NotPresent = 4,
       PositionOfOccupants_row2LeftOccupied = 5,
       PositionOfOccupants_row2RightOccupied = 6,
       PositionOfOccupants_row2MidOccupied = 7,
       PositionOfOccupants_row2NotDetectable = 8,
       PositionOfOccupants_row2NotPresent = 9,
       PositionOfOccupants_row3LeftOccupied = 10,
       PositionOfOccupants_row3RightOccupied = 11,
       PositionOfOccupants_row3MidOccupied = 12,
       PositionOfOccupants_row3NotDetectable = 13,
       PositionOfOccupants_row3NotPresent = 14,
       PositionOfOccupants_row4LeftOccupied = 15,
       PositionOfOccupants_row4RightOccupied = 16,
       PositionOfOccupants_row4MidOccupied = 17,
       PositionOfOccupants_row4NotDetectable = 18,
       PositionOfOccupants_row4NotPresent = 19,
};

typedef enum PositioningSolutionType {
    PositioningSolutionType_noPositioningSolution,
    PositioningSolutionType_sGNSS,
    PositioningSolutionType_dGNSS,
    PositioningSolutionType_sGNSSplusDR,
    PositioningSolutionType_dGNSSplusDR,
    PositioningSolutionType_dR,
    PositioningSolutionType_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} PositioningSolutionType;

typedef struct VehicleIdentification {
    BOOL wMInumber_option;
    OctetString wMInumber; /* WMInumber (1..3) */
    BOOL vDS_option;
    OctetString vDS; /* VDS (6..6) */
} VehicleIdentification;

#define EnergyStorageType_MAX_BITS 7
enum { /* Bit position of BitString: EnergyStorageType */
       EnergyStorageType_hydrogenStorage = 0,
       EnergyStorageType_electricEnergyStorage = 1,
       EnergyStorageType_liquidPropaneGas = 2,
       EnergyStorageType_compressedNaturalGas = 3,
       EnergyStorageType_diesel = 4,
       EnergyStorageType_gasoline = 5,
       EnergyStorageType_ammonia = 6,
};

enum { /* Pre-defined values of VehicleLengthValue */
       VehicleLengthValue_tenCentimeters = 1,
       VehicleLengthValue_outOfRange = 1022,
       VehicleLengthValue_unavailable = 1023,
};

typedef enum VehicleLengthConfidenceIndication {
    VehicleLengthConfidenceIndication_noTrailerPresent,
    VehicleLengthConfidenceIndication_trailerPresentWithKnownLength,
    VehicleLengthConfidenceIndication_trailerPresentWithUnknownLength,
    VehicleLengthConfidenceIndication_trailerPresenceIsUnknown,
    VehicleLengthConfidenceIndication_unavailable,
    VehicleLengthConfidenceIndication_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} VehicleLengthConfidenceIndication;

typedef struct VehicleLength {
    int32_t vehicleLengthValue; /* VehicleLengthValue (1..1023) */
    VehicleLengthConfidenceIndication vehicleLengthConfidenceIndication;
} VehicleLength;

enum { /* Pre-defined values of VehicleWidth */
       VehicleWidth_tenCentimeters = 1,
       VehicleWidth_outOfRange = 61,
       VehicleWidth_unavailable = 62,
};

#define PathHistory_MAX_SIZE 40
typedef struct PathHistory_ITS {
    PathPoint *tab;
    int32_t count;
} PathHistory_ITS;

#define EmergencyPriority_MAX_BITS 2
enum { /* Bit position of BitString: EmergencyPriority */
       EmergencyPriority_requestForRightOfWay = 0,
       EmergencyPriority_requestForFreeCrossingAtATrafficLight = 1,
};

enum { /* Pre-defined values of InformationQuality */
       InformationQuality_unavailable = 0,
       InformationQuality_lowest = 1,
       InformationQuality_highest = 7,
};

typedef enum RoadType {
    RoadType_urban_NoStructuralSeparationToOppositeLanes,
    RoadType_urban_WithStructuralSeparationToOppositeLanes,
    RoadType_nonUrban_NoStructuralSeparationToOppositeLanes,
    RoadType_nonUrban_WithStructuralSeparationToOppositeLanes,
    RoadType_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} RoadType;

enum { /* Pre-defined values of SteeringWheelAngleValue */
       SteeringWheelAngleValue_straight = 0,
       SteeringWheelAngleValue_onePointFiveDegreesToRight = -1,
       SteeringWheelAngleValue_onePointFiveDegreesToLeft = 1,
       SteeringWheelAngleValue_unavailable = 512,
};

enum { /* Pre-defined values of SteeringWheelAngleConfidence */
       SteeringWheelAngleConfidence_equalOrWithinOnePointFiveDegree = 1,
       SteeringWheelAngleConfidence_outOfRange = 126,
       SteeringWheelAngleConfidence_unavailable_ITS = 127,
};

typedef struct SteeringWheelAngle {
    int32_t steeringWheelAngleValue; /* SteeringWheelAngleValue (-511..512) */
    int32_t steeringWheelAngleConfidence; /* SteeringWheelAngleConfidence (1..127) */
} SteeringWheelAngle;

enum {
    TimestampIts_utcStartOf2004 = 0,
    TimestampIts_oneMillisecAfterUTCStartOf2004 = 1,
};

typedef enum VehicleRole {
    VehicleRole_Default,
    VehicleRole_publicTransport,
    VehicleRole_specialTransport,
    VehicleRole_dangerousGoods,
    VehicleRole_roadWork,
    VehicleRole_rescue,
    VehicleRole_emergency,
    VehicleRole_safetyCar,
    VehicleRole_agriculture,
    VehicleRole_commercial,
    VehicleRole_military,
    VehicleRole_roadOperator,
    VehicleRole_taxi,
    VehicleRole_reserved1,
    VehicleRole_reserved2,
    VehicleRole_reserved3,
    VehicleRole_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} VehicleRole;

enum { /* Pre-defined values of YawRateValue */
       YawRateValue_straight = 0,
       YawRateValue_degSec_000_01ToRight = -1,
       YawRateValue_degSec_000_01ToLeft = 1,
       YawRateValue_unavailable = 32767,
};

typedef enum YawRateConfidence_ITS {
    YawRateConfidence_degSec_000_01_ITS,
    YawRateConfidence_degSec_000_05_ITS,
    YawRateConfidence_degSec_000_10_ITS,
    YawRateConfidence_degSec_001_00_ITS,
    YawRateConfidence_degSec_005_00_ITS,
    YawRateConfidence_degSec_010_00_ITS,
    YawRateConfidence_degSec_100_00_ITS,
    YawRateConfidence_outOfRange_ITS,
    YawRateConfidence_unavailable_ITS,
    YawRateConfidence_MAX_ITS = UINT_MAX /* keep size of enum be always 4bytes */
} YawRateConfidence_ITS;

typedef struct YawRate {
    int32_t yawRateValue; /* YawRateValue (-32766..32767) */
    YawRateConfidence_ITS yawRateConfidence;
} YawRate;

typedef enum ProtectedZoneType {
    ProtectedZoneType_cenDsrcTolling,
    ProtectedZoneType_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} ProtectedZoneType;

typedef enum RelevanceDistance {
    RelevanceDistance_lessThan50m,
    RelevanceDistance_lessThan100m,
    RelevanceDistance_lessThan200m,
    RelevanceDistance_lessThan500m,
    RelevanceDistance_lessThan1000m,
    RelevanceDistance_lessThan5km,
    RelevanceDistance_lessThan10km,
    RelevanceDistance_over10km,
    RelevanceDistance_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} RelevanceDistance;

typedef enum RelevanceTrafficDirection {
    RelevanceTrafficDirection_allTrafficDirections,
    RelevanceTrafficDirection_upstreamTraffic,
    RelevanceTrafficDirection_downstreamTraffic,
    RelevanceTrafficDirection_oppositeTraffic,
    RelevanceTrafficDirection_MAX = UINT_MAX /* keep size of enum be always 4bytes */
} RelevanceTrafficDirection;

enum { /* Pre-defined values of TransmissionInterval */
       TransmissionInterval_oneMilliSecond = 1,
       TransmissionInterval_tenSeconds = 10000,
};

enum { /* Pre-defined values of ValidityDuration */
       ValidityDuration_timeOfDetection = 0,
       ValidityDuration_oneSecondAfterDetection = 1,
};

typedef struct ActionID {
    uint32_t originatingStationID; /* StationID (0..4294967295) */
    int32_t sequenceNumber; /* SequenceNumber (0..65535) */
} ActionID;

#define ItineraryPath_MAX_SIZE 40
typedef struct ItineraryPath {
    ReferencePosition *tab;
    int32_t count;
} ItineraryPath;

enum { /* Pre-defined values of ProtectedZoneRadius */
       ProtectedZoneRadius_oneMeter = 1,
};

typedef struct ProtectedCommunicationZone {
    ProtectedZoneType protectedZoneType;
    BOOL expiryTime_option;
    BigInteger expiryTime; /* TimestampIts */
    int32_t protectedZoneLatitude; /* Latitude (-900000000..900000001) */
    int32_t protectedZoneLongitude; /* Longitude (-1800000000..1800000001) */
    BOOL protectedZoneRadius_option;
    int32_t protectedZoneRadius; /* ProtectedZoneRadius (1..255) */
    BOOL protectedZoneID_option;
    int32_t protectedZoneID; /* ProtectedZoneID (0..134217727) */
} ProtectedCommunicationZone;

#define Traces_MAX_SIZE 7
typedef struct Traces {
    PathHistory_ITS *tab;
    int32_t count;
} Traces;

enum { /* Pre-defined values of NumberOfOccupants */
       NumberOfOccupants_oneOccupant = 1,
       NumberOfOccupants_unavailable = 127,
};

#define PositionOfPillars_MAX_SIZE 3
typedef struct PositionOfPillars {
    int32_t *tab; /* PosPillar (1..30) */
    int32_t count;
} PositionOfPillars;

#define RestrictedTypes_MAX_SIZE 3
typedef struct RestrictedTypes {
    int32_t *tab; /* StationType (0..255) */
    int32_t count;
} RestrictedTypes;

typedef struct EventPoint {
    DeltaReferencePosition eventPosition;
    BOOL eventDeltaTime_option;
    int32_t eventDeltaTime; /* PathDeltaTime (1..65535) */
    int32_t informationQuality; /* InformationQuality (0..7) */
} EventPoint;

#define EventHistory_MAX_SIZE 23
typedef struct EventHistory {
    EventPoint *tab;
    int32_t count;
} EventHistory;

#define ProtectedCommunicationZonesRSU_MAX_SIZE 16
typedef struct ProtectedCommunicationZonesRSU {
    ProtectedCommunicationZone *tab;
    int32_t count;
} ProtectedCommunicationZonesRSU;

typedef struct CenDsrcTollingZone {
    int32_t protectedZoneLatitude; /* Latitude (-900000000..900000001) */
    int32_t protectedZoneLongitude; /* Longitude (-1800000000..1800000001) */
    BOOL cenDsrcTollingZoneID_option;
    int32_t cenDsrcTollingZoneID; /* CenDsrcTollingZoneID */
} CenDsrcTollingZone;

#ifdef __cplusplus
}
#endif

#endif /* __ITSMSG_CDD_H__ */
