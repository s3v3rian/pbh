/**
 *******************************************************************************
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * @file    gn_security_user.h
 * @brief
 * @author  Unex
 *******************************************************************************
 */

#ifndef __GN_SECURITY_USER_H__
#define __GN_SECURITY_USER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "its_sec_usr.h"


/*
 *******************************************************************************
 * Constant value definition
 *******************************************************************************
 */

#define GN_SEC_SSP_BUF_SIZE ITS_SSP_MAX_LEN /* According to ETSI TS 103 09u1 V1.3.1 (2017-10). */


/*
 *******************************************************************************
 * Data type definition
 *******************************************************************************
 */

typedef enum gn_sec_encap_type {
    GN_SEC_ENCAP_PKT_UNSECURE = 0, /* No Encapsulation, unsecured mode. */
    GN_SEC_ENCAP_PKT_BY_AID_PROFILE, /* Encapsulate packet according to the AID profile. */
    GN_SEC_ENCAP_TYPE_MAX_NUM /* Can't be used */
} gn_sec_encap_type_e;


/*
 *******************************************************************************
 * Global variables
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Public functions declaration
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Private functions declaration
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Public functions
 *******************************************************************************
 */

/**
 * function_example - Function example
 *
 * @param   [in]    input       Example input.
 * @param   [out]   *p_output   Example output.
 *
 * @return  [int]   Function executing result.
 * @retval  [0]     Success.
 * @retval  [-1]    Fail.
 *
 */


/*
 *******************************************************************************
 * Private functions
 *******************************************************************************
 */


#endif  // __GN_SECURITY_USER_H__
