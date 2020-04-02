/* ITS-G5 RTCMEM definitions
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef __ITSMSG_RTCMEM_H__
#define __ITSMSG_RTCMEM_H__


#include "itsmsg_cdd.h"
#include "j2735_rtcm.h"
#include "itsmsg_iso19091.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RTCMEM {
    ItsPduHeader header;
    RTCMcorrections rtcmc;
} RTCMEM;

#ifdef __cplusplus
}
#endif

#endif /* __ITSMSG_RTCMEM_H__ */
