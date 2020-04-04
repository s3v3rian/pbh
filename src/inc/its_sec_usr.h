/*
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#ifndef __ITS_SEC_USR_H__
#define __ITS_SEC_USR_H__

#define ITS_SSP_MAX_LEN 32
/* ITS-AID Value */
#define ITS_SEC_INVALID_AID 0xFFFFFFFF
#define ITS_SEC_CAM_AID 36
#define ITS_SEC_DEN_AID 37
#define ITS_SEC_GN_MNG_AID 141

#define IS_DECAP_FAIL(rc) ((rc > ITS_SEC_SUCCESS) ? 1 : 0)

typedef enum {
    ITS_SEC_NA = 0, /* non-security format */
    ITS_SEC_SUCCESS, /* Decap EtsiTs103097 SignedData / EncryptedData success*/
    ITS_SEC_FAIL, /* Decap fail */
} isc_decap_status;

#endif
