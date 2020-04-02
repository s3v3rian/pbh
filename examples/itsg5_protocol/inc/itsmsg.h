/* ITS-G5 message definitions
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef __ITSMSG_H__
#define __ITSMSG_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ITSmsgID {
    DENM_Id = 1,
    CAM_Id = 2,
    /*  POI_Id = 3, */
    SPATEM_Id = 4,
    MAPEM_Id = 5,
    /*  IVI_Id = 6, */
    /*  EV_RSR_Id = 7 */
    RTCMEM_Id = 13
} ITSmsgID;

#include "itsmsg_denm.h"
#include "itsmsg_cam.h"
#include "itsmsg_spatem.h"
#include "itsmsg_mapem.h"
#include "itsmsg_rtcmem.h"

#ifdef __cplusplus
}
#endif

#endif /* __ITSMSG_H__ */
