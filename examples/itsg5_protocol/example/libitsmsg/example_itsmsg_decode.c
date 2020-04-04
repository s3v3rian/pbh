#include <stdio.h>
#include <stdlib.h>

#include "itsmsg_codec.h"
/*
rec1value MAPEM ::= {
  header {
    protocolVersion currentVersion,
    messageID map,
    stationID 1110
  },
  map {
    timeStamp 0,
    msgIssueRevision 1,
    layerType parkingAreaData,
    layerID 26,
    roadSegments {
      {
        id {
          id 0
        },
        revision 21,
        refPoint {
          lat -899999997,
          long -1799999939,
          regional {
            {
              regionId 3,
              regExtValue Position3D-addGrpC : {
                altitude {
                  altitudeValue -99769,
                  altitudeConfidence alt-005-00
                }
              }
            }
          }
        },
        laneWidth 12633,
        speedLimits {
          {
            type vehicleMaxSpeed,
            speed 6397
          }
        },
        roadLaneSet {
          {
            laneID 0,
            laneAttributes {
              directionalUse '10'B,
              sharedWith '0100111100'B,
              laneType crosswalk : '1101100011010010'B
            },
            nodeList nodes : {
              {
                delta node-LatLon : {
                  lon -1799999753,
                  lat -899999981
                },
                attributes {
                  localNode {
                    hydrantPresent,
                    roundedCapStyleB,
                    curbPresentAtStepOff,
                    divergePoint,
                    stopLine,
                    curbPresentAtStepOff,
                    curbPresentAtStepOff,
                    downstreamStopLine
                  },
                  dWidth -295
                }
              },
              {
                delta node-XY1 : {
                  x 97,
                  y -508
                }
              },
              {
                delta node-LatLon : {
                  lon -357115427,
                  lat -899999999
                },
                attributes {
                  localNode {
                    roundedCapStyleB,
                    curbPresentAtStepOff,
                    stopLine
                  },
                  data {
                    laneAngle : -177,
                    laneAngle : -179,
                    speedLimits : {
                      {
                        type vehiclesWithTrailersMaxSpeed,
                        speed 2485
                      },
                      {
                        type truckMaxSpeed,
                        speed 574
                      }
                    },
                    laneCrownPointRight : -73,
                    laneCrownPointCenter : -127
                  },
                  dWidth -512
                }
              },
              {
                delta node-XY2 : {
                  x -1022,
                  y 895
                }
              },
              {
                delta node-XY3 : {
                  x -1682,
                  y 771
                }
              },
              {
                delta regional : {
                  regionId 2,
                  regExtValue NodeOffsetPointXY-addGrpB : posB : {
                    lon {
                      d -179,
                      m 2,
                      s 735
                    },
                    lat {
                      d -87,
                      m 0,
                      s 3
                    }
                  }
                },
                attributes {
                  localNode {
                    downstreamStartNode,
                    roundedCapStyleB,
                    roundedCapStyleB,
                    downstreamStartNode,
                    stopLine,
                    roundedCapStyleB,
                    curbPresentAtStepOff
                  },
                  disabled {
                    rumbleStripPresent,
                    mergingLaneLeft,
                    adjacentBikeLaneOnLeft
                  },
                  data {
                    speedLimits : {
                      {
                        type truckMinSpeed,
                        speed 0
                      }
                    },
                    laneCrownPointLeft : -95,
                    laneCrownPointCenter : -58,
                    laneCrownPointCenter : 53,
                    laneAngle : -173,
                    laneCrownPointRight : -35
                  },
                  dElevation -510
                }
              },
              {
                delta node-XY6 : {
                  x -32766,
                  y -32768
                },
                attributes {
                  localNode {
                    safeIsland,
                    stopLine,
                    safeIsland,
                    downstreamStopLine,
                    hydrantPresent,
                    curbPresentAtStepOff,
                    downstreamStartNode,
                    mergePoint
                  },
                  data {
                    laneCrownPointCenter : -97,
                    speedLimits : {
                      {
                        type vehiclesWithTrailersMinSpeed,
                        speed 2266
                      },
                      {
                        type maxSpeedInSchoolZone,
                        speed 4065
                      }
                    },
                    speedLimits : {
                      {
                        type vehicleNightMaxSpeed,
                        speed 3
                      },
                      {
                        type truckMaxSpeed,
                        speed 250
                      },
                      {
                        type maxSpeedInConstructionZone,
                        speed 4100
                      },
                      {
                        type maxSpeedInConstructionZone,
                        speed 726
                      },
                      {
                        type vehicleNightMaxSpeed,
                        speed 1
                      },
                      {
                        type maxSpeedInSchoolZoneWhenChildrenArePresent,
                        speed 2
                      },
                      {
                        type truckMinSpeed,
                        speed 1
                      },
                      {
                        type vehicleMinSpeed,
                        speed 143
                      }
                    }
                  },
                  dWidth -9,
                  dElevation 448
                }
              },
              {
                delta node-XY2 : {
                  x -1023,
                  y -492
                }
              },
              {
                delta node-XY2 : {
                  x -1024,
                  y -640
                }
              },
              {
                delta node-XY2 : {
                  x -1023,
                  y -973
                }
              }
            },
            connectsTo {
              {
                connectingLane {
                  lane 11
                },
                remoteIntersection {
                  region 179,
                  id 1531
                }
              },
              {
                connectingLane {
                  lane 9,
                  maneuver '010011110011'B
                },
                remoteIntersection {
                  id 40
                },
                signalGroup 23
              },
              {
                connectingLane {
                  lane 3,
                  maneuver '011001101100'B
                },
                remoteIntersection {
                  region 81,
                  id 3244
                },
                signalGroup 55,
                userClass 103
              }
            },
            overlays {
              2,
              100
            }
          }
        }
      }
    },
    dataParameters {
      processMethod "999",
      processAgency "google.com",
      lastCheckedDate "199/44/3"
    },
    regional {
      {
        regionId 3,
        regExtValue MapData-addGrpC : {
          signalHeadLocations {
            {
              nodeXY node-XY1 : {
                x -488,
                y -449
              },
              nodeZ 100,
              signalGroupID 3
            },
            {
              nodeXY node-XY4 : {
                x -4093,
                y -3711
              },
              nodeZ 100,
              signalGroupID 5
            },
            {
              nodeXY node-XY2 : {
                x -230,
                y -1024
              },
              nodeZ 100,
              signalGroupID 1
            },
            {
              nodeXY node-XY3 : {
                x -1552,
                y 828
              },
              nodeZ 100,
              signalGroupID 77
            },
            {
              nodeXY node-LatLon : {
                lon -1767470150,
                lat -597772339
              },
              nodeZ 100,
              signalGroupID 202
            },
            {
              nodeXY node-LatLon : {
                lon -848354193,
                lat -899999617
              },
              nodeZ 100,
              signalGroupID 5
            },
            {
              nodeXY node-LatLon : {
                lon -1799995216,
                lat -899997626
              },
              nodeZ 100,
              signalGroupID 0
            },
            {
              nodeXY node-XY2 : {
                x -1023,
                y 423
              },
              nodeZ 100,
              signalGroupID 28
            },
            {
              nodeXY node-LatLon : {
                lon -1799999094,
                lat -899999995
              },
              nodeZ 100,
              signalGroupID 21
            }
          }
        }
      }
    }
  }
}
*/

/* the above data in uper format */
const uint8_t mapem_uper_data[] = {
    0x01, 0x05, 0x00, 0x00, 0x04, 0x56, 0x76, 0x80, 0x00, 0x00, 0x16, 0x34,
    0x03, 0x00, 0x00, 0x0a, 0x90, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x07,
    0x80, 0x18, 0x20, 0x00, 0x39, 0xe0, 0x03, 0x15, 0x90, 0x2e, 0x3f, 0x40,
    0x01, 0x80, 0x12, 0x78, 0x3b, 0x1a, 0x41, 0x0e, 0x00, 0x00, 0x00, 0xf6,
    0x00, 0x00, 0x00, 0x26, 0x89, 0xd6, 0x35, 0x14, 0x2a, 0x51, 0x8d, 0x90,
    0x4c, 0x20, 0x23, 0x95, 0x80, 0x2a, 0xf7, 0x00, 0x00, 0x00, 0x00, 0xa6,
    0x21, 0xa8, 0x31, 0x00, 0x68, 0x01, 0x51, 0x5a, 0x6d, 0x50, 0x23, 0xe3,
    0x37, 0x10, 0x10, 0x00, 0x20, 0x0b, 0xbf, 0x88, 0x5b, 0xac, 0x0d, 0xe0,
    0x41, 0x08, 0x04, 0x21, 0x6f, 0x81, 0x80, 0x00, 0x30, 0x0d, 0x58, 0xe3,
    0x19, 0xc2, 0x35, 0x22, 0xe0, 0xc6, 0xd5, 0x03, 0x80, 0x00, 0x88, 0x45,
    0x18, 0x6d, 0x50, 0x0e, 0x6b, 0xa0, 0x13, 0x40, 0x00, 0x80, 0x00, 0x13,
    0xba, 0x42, 0x93, 0x2d, 0x47, 0x22, 0x11, 0xf5, 0x15, 0x23, 0x68, 0x2f,
    0xe1, 0x57, 0x30, 0x00, 0xd0, 0x0f, 0xa1, 0xc0, 0x10, 0x62, 0xd6, 0x30,
    0x00, 0x44, 0x00, 0x23, 0x80, 0x04, 0x80, 0x8f, 0x7d, 0xfc, 0x00, 0x80,
    0x14, 0x28, 0x10, 0x00, 0x60, 0x02, 0x00, 0x41, 0x99, 0x40, 0x2e, 0x01,
    0x66, 0x0b, 0xf7, 0x90, 0x94, 0xf3, 0x00, 0x14, 0x0b, 0xf4, 0x0d, 0x9b,
    0x20, 0x0a, 0x21, 0x95, 0x86, 0xec, 0xe4, 0x09, 0x91, 0xc0, 0x4e, 0x5c,
    0xb9, 0x09, 0xcf, 0xbf, 0x7e, 0x7d, 0x99, 0x57, 0x63, 0xdf, 0xb4, 0x1d,
    0x8b, 0x97, 0x2b, 0xda, 0x34, 0x5e, 0xcc, 0x03, 0x4e, 0x48, 0x00, 0x60,
    0x3f, 0x64, 0x00, 0x06, 0x60, 0x03, 0x0c, 0x0b, 0x20, 0x00, 0x51, 0x63,
    0x40, 0x01, 0x90, 0x00, 0x09, 0x0f, 0x85, 0x9e, 0x32, 0x00, 0x4d, 0x60,
    0x1f, 0x05, 0xdb, 0x92, 0x40, 0x74, 0x19, 0xac, 0x80, 0x32, 0x98, 0xe2,
    0xe3, 0xd9, 0xb8, 0x00, 0x00, 0x0b, 0xfb, 0x20, 0x00, 0x56, 0x00, 0x00,
    0x12, 0xaf, 0x00, 0x00, 0x12, 0x8c, 0xc8, 0x00, 0x00, 0x40, 0x0d, 0xa7,
    0x64, 0x00, 0x38, 0xc0, 0x00, 0x00, 0x71, 0x20, 0x00, 0x00, 0x01, 0x59,
    0x00, 0x0a, 0x80};
const uint32_t mapem_uper_data_len = 303;

void print_MAP(MapData *map)
{
    int i, j, fbs;
    RoadSegment *road;
    GenericLane *lane;

    printf("MAP:\n");
    if (map->timeStamp_option) {
        printf("  timeStamp: %u\n", map->timeStamp);
    }
    printf("  msgIssueRevision: %u\n", map->msgIssueRevision);
    /* the number of items in "sequence of" could be gotten by field, "count"  */
    for (i = 0; i < map->roadSegments.count; i++) {
        road = &(map->roadSegments.tab[i]);
        printf("  RoadSegment[%d]:\n", i);
        printf("    Id: %u\n", road->id.id);
        for (j = 0; j < road->roadLaneSet.count; j++) {
            lane = &(road->roadLaneSet.tab[j]);
            printf("    Lane[%d]:\n", j);
            printf("      directionalUse: ");
            fbs = asn1_bstr_ffs(&(lane->laneAttributes.directionalUse));
            switch (fbs) {
                case LaneDirection_ingressPath:
                    printf("LaneDirection_ingressPath\n");
                    break;
                case LaneDirection_egressPath:
                    printf("LaneDirection_egressPath\n");
                    break;
                default:
                    printf("value is not defined\n");
                    break;
            }
        }
    }
}

void print_SPAT(SPAT *spat)
{
    printf("SPAT:\n");
    printf("  num. of intersections: %u\n", spat->intersections.count);
}

#define ERR_MSG_SZ 256
#ifdef __THREADX__
#define MALLOC(sz) simple_malloc(sz)
#define CALLOC(n, sz) simple_calloc((n), (sz))
#define FREE(ptr) simple_free(ptr)
#else
#define MALLOC(sz) malloc(sz)
#define CALLOC(n, sz) calloc((n), (sz))
#define FREE(ptr) free(ptr)
#endif

void ex_itsmsg_decode()
{
    int ret;
    ITSMsgCodecErr err;

    /* a pointer to containing decoded msg */
    ItsPduHeader *p_itshdr;

    printf("--- %s ---\n", __FILE__);

    err.msg_size = ERR_MSG_SZ;
    err.msg = MALLOC(err.msg_size);
    /* does the decoding */
    ret = itsmsg_decode(&p_itshdr, mapem_uper_data, mapem_uper_data_len, &err);

    if (ret < 0) {
        /* handling the decoding error */
        printf("decode msg error\n");
    }
    else {
        /* do the processing by corresponding message handler */
        switch (p_itshdr->messageID) {
            case MAPEM_Id:
                print_MAP(&((MAPEM *)p_itshdr)->map);
                break;
            case SPATEM_Id:
                print_SPAT(&((SPATEM *)p_itshdr)->spat);
                break;
            default:
                printf("unsupported message\n");
        }
        /* free the decoded msg only when decoding without errors */
        itsmsg_free(p_itshdr);
    }

    FREE(err.msg);
    return;
}
