/* ITS-G5 MAPEM definitions
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef __ITSMSG_MAPEM_H__
#define __ITSMSG_MAPEM_H__


#include "itsmsg_cdd.h"
#include "j2735_map.h"
#include "itsmsg_iso19091.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MAPEM {
    ItsPduHeader header;
    MapData map;
} MAPEM;

#ifdef __cplusplus
}
#endif

#endif /* __ITSMSG_MAPEM_H__ */
