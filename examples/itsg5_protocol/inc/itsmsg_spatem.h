/* ITS-G5 SPATEM definitions
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef __ITSMSG_SPATEM_H__
#define __ITSMSG_SPATEM_H__


#include "itsmsg_cdd.h"
#include "j2735_spat.h"
#include "itsmsg_iso19091.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SPATEM {
    ItsPduHeader header;
    SPAT spat;
} SPATEM;

#ifdef __cplusplus
}
#endif

#endif /* __ITSMSG_SPATEM_H__ */
