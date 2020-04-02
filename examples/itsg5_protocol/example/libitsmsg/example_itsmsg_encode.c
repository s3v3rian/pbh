#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __THREADX__
#include <tx_api.h>
#endif
#include "itsmsg_codec.h"

#define ERR_MSG_SZ 256
#ifdef __THREADX__
static TX_BYTE_POOL simple_mem_pool;
#define MEM_POOL_SIZE 1 * 1024 * 1024
static unsigned char mem_pool_buf[MEM_POOL_SIZE];
static int flag_init = 0;
#define MALLOC(sz) simple_malloc(sz)
#define CALLOC(n, sz) simple_calloc((n), (sz))
#define FREE(ptr) simple_free(ptr)
#else
#define MALLOC(sz) malloc(sz)
#define CALLOC(n, sz) calloc((n), (sz))
#define FREE(ptr) free(ptr)
#endif

#ifdef __THREADX__
void *simple_malloc(size_t size)
{
    void *ptr;

    if (flag_init == 0) {
        flag_init = 1;
        tx_byte_pool_create(&simple_mem_pool, "simple_mem_pool", (void *)mem_pool_buf, MEM_POOL_SIZE);
    }

    tx_byte_allocate(&simple_mem_pool, (void **)&ptr, size, TX_WAIT_FOREVER);

    return ptr;
}

void simple_free(void *ptr)
{
    UINT tx_rc;

    tx_rc = tx_byte_release(ptr);
    if (tx_rc != TX_SUCCESS) {
        printf("free = %d\n", tx_rc);
    }

    return;
}

void *simple_calloc(unsigned int nmemb, unsigned int size)
{
    void *ptr = NULL;
    UINT tx_rc;

    if (flag_init == 0) {
        flag_init = 1;
        tx_byte_pool_create(&simple_mem_pool, "simple_mem_pool", (void *)mem_pool_buf, MEM_POOL_SIZE);
    }

    tx_rc = tx_byte_allocate(&simple_mem_pool, (void **)&ptr, nmemb * size, TX_WAIT_FOREVER);
    if (tx_rc != TX_SUCCESS) {
        printf("CALLOC = %d\n", tx_rc);
    }
    (void)memset(ptr, 0, nmemb * size);

    return ptr;
}
#endif

static void encode_example3()
{
    int buf_len;
    uint8_t *buf;
    ITSMsgCodecErr err;
    /* a pointer to containing msg to be encoded */
    MAPEM *mapem;
    MapData *map;
    RoadSegment *road;
    GenericLane *lane;

    printf("Encode Example 3:\n");

    /* all fields which should be allocated before using are allocated recursivly */
    mapem = (MAPEM *)itsmsg_prealloc(MAPEM_Id, 1);

    /*its header */
    mapem->header.stationID = 999;

    /* map */
    map = &(mapem->map);
    map->msgIssueRevision = 1;

    map->roadSegments_option = TRUE;
    //map->roadSegments.tab = CALLOC(1, sizeof(RoadSegment));
    /* the memory is allocated by itsmsg_prealloc() and the size is element size * max. count */
    /* set the count to the number of elements which contain valid values */
    map->roadSegments.count = 1;
    road = &(map->roadSegments.tab[0]);
    road->id.id = 12345;
    road->revision = 1;
    road->refPoint.lat = 1234;
    road->refPoint.Long = 56778;

    //road->roadLaneSet.tab = CALLOC(1, sizeof(GenericLane));
    /* the memory is allocated by itsmsg_prealloc() and the size is element size * max. count */
    /* set the count to the number of elements which contain valid values */
    road->roadLaneSet.count = 1;
    lane = &(road->roadLaneSet.tab[0]);
    lane->laneID = 67;
    //asn1_bstr_alloc_set_bit(&(lane->laneAttributes.directionalUse), LaneDirection_MAX_BITS, LaneDirection_ingressPath);
    asn1_bstr_set_bit(&(lane->laneAttributes.directionalUse), LaneDirection_ingressPath);
    //asn1_bstr_alloc_set_bit(&(lane->laneAttributes.sharedWith), LaneSharing_MAX_BITS, LaneSharing_busVehicleTraffic);
    asn1_bstr_set_bit(&(lane->laneAttributes.sharedWith), LaneSharing_busVehicleTraffic);
    lane->laneAttributes.laneType.choice = LaneTypeAttributes_vehicle;
    lane->laneAttributes.laneType.choice = LaneTypeAttributes_vehicle;
    //asn1_bstr_alloc_set_bit(&(lane->laneAttributes.laneType.u.vehicle), LaneAttributes_Vehicle_MAX_BITS, LaneAttributes_Vehicle_restrictedToBusUse);
    asn1_bstr_set_bit(&(lane->laneAttributes.laneType.u.vehicle), LaneAttributes_Vehicle_restrictedToBusUse);
    lane->nodeList.choice = NodeListXY_nodes;
    //lane->nodeList.u.nodes.tab = CALLOC(1, sizeof(NodeXY));
    /* the dataframe_prealloc() is similar as itsmsg_prealloc(), the elements are recursively allocated */
    j2735_dataframe_prealloc(NodeListXY_DF, &(lane->nodeList));
    lane->nodeList.u.nodes.count = 2;
    lane->nodeList.u.nodes.tab[0].delta.choice = NodeOffsetPointXY_node_XY1;
    lane->nodeList.u.nodes.tab[0].delta.u.node_XY1.x = 256;
    lane->nodeList.u.nodes.tab[0].delta.u.node_XY1.y = -256;
    lane->nodeList.u.nodes.tab[1].delta.choice = NodeOffsetPointXY_node_XY1;
    lane->nodeList.u.nodes.tab[1].delta.u.node_XY1.x = 11;
    lane->nodeList.u.nodes.tab[1].delta.u.node_XY1.y = -256;

    err.msg_size = ERR_MSG_SZ;
    err.msg = MALLOC(err.msg_size);
    buf_len = itsmsg_encode(&buf, (ItsPduHeader *)mapem, &err);
    if (buf_len <= 0) {
        printf("  failed to encode the msg\n");
        printf("  [error msg] %s\n", err.msg);
    }
    else {
        printf("  encode successfully\n");
    }

    /* free the memory for encoding */
    //asn1_bstr_free(&(lane->laneAttributes.directionalUse));
    //asn1_bstr_free(&(lane->laneAttributes.sharedWith));
    //asn1_bstr_free(&(lane->laneAttributes.laneType.u.vehicle));
    //free(lane->nodeList.u.nodes.tab);
    j2735_dataframe_dealloc(NodeListXY_DF, &(lane->nodeList));
    //free(road->roadLaneSet.tab);
    //free(map->roadSegments.tab);
    itsmsg_dealloc(MAPEM_Id, mapem);
    FREE(err.msg);
    itsmsg_buf_free(buf);
}

void encode_example2()
{
    int buf_len;
    uint8_t *buf;
    ITSMsgCodecErr err;
    /* a pointer to containing msg to be encoded */
    MAPEM *mapem;
    MapData *map;
    RoadSegment *road;
    GenericLane *lane;

    printf("Encode Example 2:\n");

    mapem = (MAPEM *)CALLOC(1, sizeof(MAPEM));

    /*its header */
    mapem->header.protocolVersion = 1;
    mapem->header.messageID = MAPEM_Id;
    mapem->header.stationID = 999;

    /* map */
    map = &(mapem->map);
    map->msgIssueRevision = 1;
    map->roadSegments_option = TRUE;
    map->roadSegments.tab = CALLOC(1, sizeof(RoadSegment));
    map->roadSegments.count = 1;

    road = &(map->roadSegments.tab[0]);
    road->id.id = 12345;
    road->revision = 1;
    road->refPoint.lat = 1234;
    road->refPoint.Long = 56778;
    road->roadLaneSet.tab = CALLOC(1, sizeof(GenericLane));
    road->roadLaneSet.count = 1;

    lane = &(road->roadLaneSet.tab[0]);
    lane->laneID = 67;
    asn1_bstr_alloc_set_bit(&(lane->laneAttributes.directionalUse), LaneDirection_MAX_BITS, LaneDirection_ingressPath);
    /* not set the field, sharedWith */
    //asn1_bstr_alloc_set_bit(&(lane->laneAttributes.sharedWith), LaneSharing_MAX_BITS, LaneSharing_busVehicleTraffic);
    lane->laneAttributes.laneType.choice = LaneTypeAttributes_vehicle;
    lane->laneAttributes.laneType.choice = LaneTypeAttributes_vehicle;
    asn1_bstr_alloc_set_bit(&(lane->laneAttributes.laneType.u.vehicle), LaneAttributes_Vehicle_MAX_BITS, LaneAttributes_Vehicle_restrictedToBusUse);
    lane->nodeList.choice = NodeListXY_nodes;
    lane->nodeList.u.nodes.tab = CALLOC(2, sizeof(NodeXY));
    lane->nodeList.u.nodes.count = 2;
    lane->nodeList.u.nodes.tab[0].delta.choice = NodeOffsetPointXY_node_XY1;
    lane->nodeList.u.nodes.tab[0].delta.u.node_XY1.x = 256;
    lane->nodeList.u.nodes.tab[0].delta.u.node_XY1.y = -256;
    lane->nodeList.u.nodes.tab[1].delta.choice = NodeOffsetPointXY_node_XY1;
    lane->nodeList.u.nodes.tab[1].delta.u.node_XY1.x = 11;
    lane->nodeList.u.nodes.tab[1].delta.u.node_XY1.y = -256;

    err.msg_size = ERR_MSG_SZ;
    err.msg = MALLOC(err.msg_size);
    buf_len = itsmsg_encode(&buf, (ItsPduHeader *)mapem, &err);
    if (buf_len <= 0) {
        printf("  failed to encode the msg\n");
        printf("  [error msg] %s\n", err.msg);
    }
    else {
        printf("  encode successfully\n");
    }

    /* free the memory for encoding */
    asn1_bstr_free(&(lane->laneAttributes.directionalUse));
    //asn1_bstr_free(&(lane->laneAttributes.sharedWith));
    asn1_bstr_free(&(lane->laneAttributes.laneType.u.vehicle));
    FREE(lane->nodeList.u.nodes.tab);
    FREE(road->roadLaneSet.tab);
    FREE(map->roadSegments.tab);
    FREE(mapem);
    FREE(err.msg);
    //itsmsg_buf_free(buf);
}

void encode_example1()
{
    int buf_len;
    uint8_t *buf;
    ITSMsgCodecErr err;
    /* a pointer to containing msg to be encoded */
    MAPEM *mapem;
    MapData *map;
    RoadSegment *road;
    GenericLane *lane;

    printf("Encode Example 1:\n");

    mapem = (MAPEM *)CALLOC(1, sizeof(MAPEM));

    /*its header */
    mapem->header.protocolVersion = 1;
    mapem->header.messageID = MAPEM_Id;
    mapem->header.stationID = 999;

    /* map */
    map = &(mapem->map);
    map->msgIssueRevision = 1;
    map->roadSegments_option = TRUE;
    map->roadSegments.tab = CALLOC(1, sizeof(RoadSegment));
    map->roadSegments.count = 1;

    road = &(map->roadSegments.tab[0]);
    road->id.id = 12345;
    road->revision = 1;
    road->refPoint.lat = 1234;
    road->refPoint.Long = 56778;
    road->roadLaneSet.tab = CALLOC(1, sizeof(GenericLane));
    road->roadLaneSet.count = 1;

    lane = &(road->roadLaneSet.tab[0]);
    lane->laneID = 67;
    asn1_bstr_alloc_set_bit(&(lane->laneAttributes.directionalUse), LaneDirection_MAX_BITS, LaneDirection_ingressPath);
    asn1_bstr_alloc_set_bit(&(lane->laneAttributes.sharedWith), LaneSharing_MAX_BITS, LaneSharing_busVehicleTraffic);
    lane->laneAttributes.laneType.choice = LaneTypeAttributes_vehicle;
    lane->laneAttributes.laneType.choice = LaneTypeAttributes_vehicle;
    asn1_bstr_alloc_set_bit(&(lane->laneAttributes.laneType.u.vehicle), LaneAttributes_Vehicle_MAX_BITS, LaneAttributes_Vehicle_restrictedToBusUse);
    lane->nodeList.choice = NodeListXY_nodes;
    lane->nodeList.u.nodes.tab = CALLOC(2, sizeof(NodeXY));
    lane->nodeList.u.nodes.count = 2;
    lane->nodeList.u.nodes.tab[0].delta.choice = NodeOffsetPointXY_node_XY1;
    lane->nodeList.u.nodes.tab[0].delta.u.node_XY1.x = 256;
    lane->nodeList.u.nodes.tab[0].delta.u.node_XY1.y = -256;
    lane->nodeList.u.nodes.tab[1].delta.choice = NodeOffsetPointXY_node_XY1;
    lane->nodeList.u.nodes.tab[1].delta.u.node_XY1.x = 11;
    lane->nodeList.u.nodes.tab[1].delta.u.node_XY1.y = -256;

    err.msg_size = ERR_MSG_SZ;
    err.msg = MALLOC(err.msg_size);
    buf_len = itsmsg_encode(&buf, (ItsPduHeader *)mapem, &err);
    if (buf_len <= 0) {
        printf("  failed to encode the msg\n");
        printf("  [error msg] %s\n", err.msg);
    }
    else {
        printf("  encode successfully\n");
    }

    /* free the memory for encoding */
    asn1_bstr_free(&(lane->laneAttributes.directionalUse));
    asn1_bstr_free(&(lane->laneAttributes.sharedWith));
    asn1_bstr_free(&(lane->laneAttributes.laneType.u.vehicle));
    FREE(lane->nodeList.u.nodes.tab);
    FREE(road->roadLaneSet.tab);
    FREE(map->roadSegments.tab);
    FREE(mapem);
    FREE(err.msg);
    itsmsg_buf_free(buf);
}

void ex_itsmsg_encode()
{
    printf("--- %s ---\n", __FILE__);
    /* fill and alloc. memory on demand */
    encode_example1();
    /* init. error message structure to log the error for debugging */
    encode_example2();
    /* prealloc. memory by memory prealloc assistant and fill values on demand */
    encode_example3();

    return;
}
