/**
 *******************************************************************************
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * @file    error_code_enum.h
 * @brief   This header is generated automatically for Unex V2X SDK v1.3.0
 *          please do not modify this file manually.
 * @author  Unex
 *******************************************************************************
 */

#ifndef __ERROR_CODE_ENUM_H__
#define __ERROR_CODE_ENUM_H__

/* Error code definition for Unex V2X SDK v1.3.0 */
typedef enum error_code {

    /* generic */
    SUCCESS                                                          = 0, /* No error */
    ERROR_NULL_POINTER                                               = -1, /* Null pointer access */
    ERROR_INIT_FAIL                                                  = -2, /* Initialization failed */
    ERROR_NOT_INIT                                                   = -3, /* Not yet initialized */
    ERROR_INVALID_ARGS                                               = -4, /* Value of argument is invalid */
    ERROR_NO_MEMORY                                                  = -5, /* No more memory */
    ERROR_TIMEOUT                                                    = -6, /* Timeout */
    ERROR_UNSPECIFIED                                                = -7, /* Unspecified error */
    ERROR_UNSUPPORT_TYPE                                             = -8, /* Unsupported type */
    ERROR_NO_RESOURCE                                                = -9, /* No resource */
    ERROR_BUSY                                                       = -10, /* Busy */
    ERROR_CONN_FAIL                                                  = -11, /* Connect failed */

    /* poti */
    ERROR_POTI_SHM_VER_ERROR                                         = -1024, /* Version of poti shm is wrong */
    ERROR_NOT_SET_TAI_OFFSET                                         = -1025, /* System TAI offset is not set */

    /* gn */
    ERROR_GN_HDLR_IPC_ALLOCATE                                       = -2048, /* Can't allocate GN IPC interface */
    ERROR_GN_HDLR_IPC_SET_CFG                                        = -2049, /* Can't setup GN IPC interface */
    ERROR_GN_HDLR_IPC_INVALID_IDX                                    = -2050, /* Invalid GN handler index */
    ERROR_GN_HDLR_IPC_INVALID_LOCK                                   = -2051, /* Invalid GN IPC lock */
    ERROR_GN_HDLR_IPC_CONN_PUBLIC_IF                                 = -2052, /* Can't connect to GN public channel, please make sure GN daemon is activated */
    ERROR_GN_HDLR_IPC_VERSION_MISMATCH                               = -2053, /* The IPC version is mismatched */
    ERROR_GN_HDLR_IPC_REQ_SIZE_INVALID                               = -2054, /* Wrong GN IPC request message size */
    ERROR_GN_HDLR_IPC_REQ_ISSUE_FAIL                                 = -2055, /* Can't issue GN IPC request */
    ERROR_GN_HDLR_IPC_RSP_SIZE_INVALID                               = -2056, /* Wrong GN IPC response message size */
    ERROR_GN_HDLR_IPC_RSP_HDR_INVALID                                = -2057, /* Wrong GN IPC response content */
    ERROR_GN_HDLR_IPC_RSP_REJECT                                     = -2058, /* Rejected by GN service */

    ERROR_GN_HDLR_MGNT_IPC_ESTI_FAIL                                 = -2080, /* Can't estimate GN MGNT service connection */
    ERROR_GN_HDLR_MGNT_IPC_IDX_INVALID                               = -2081, /* Wrong GN MGNT service connection index */
    ERROR_GN_HDLR_MGNT_SECURITY_MIB_DISABLED                         = -2082, /* btp_config.security.type is BY_PROFILE, but "is_mib_its_gn_security_enabled" is false in gnd.json */
    ERROR_GN_HDLR_MGNT_SECURITY_TYPE_INVALID                         = -2083, /* Incorrect security type */
    ERROR_GN_HDLR_MGNT_SECURITY_TYPE_UNSECURED_OPERATION             = -2084, /* btp_config.security.type is UNSECURED, but "is_mib_its_gn_security_enabled" is true in gnd.json */
    ERROR_GN_HDLR_MGNT_SECURITY_AID_INVALID                          = -2085, /* This ITS-AID is invalid and should not be used */
    ERROR_GN_HDLR_MGNT_SECURITY_AID_CONFLICT                         = -2086, /* This ITS-AID is occupied by other APP */

    ERROR_GN_HDLR_TX_IPC_ESTI_FAIL                                   = -2112, /* Can't estimate GN TX service connection */
    ERROR_GN_HDLR_TX_IPC_IDX_INVALID                                 = -2113, /* Wrong GN TX service connection index */
    ERROR_GN_HDLR_TX_PAYLOAD_EMPTY                                   = -2114, /* TX payload is empty */
    ERROR_GN_HDLR_TX_PAYLOAD_TOO_LONG                                = -2115, /* TX payload size is too large */
    ERROR_GN_HDLR_TX_NETWORK_HDR_TYPE_UNSUPPORTED                    = -2116, /* Unsupported TX network header type */
    ERROR_GN_HDLR_TX_TRANSPORT_HDR_TYPE_UNSUPPORTED                  = -2117, /* Unsupported TX transport header type */
    ERROR_GN_HDLR_TX_SRC_PORT_NOT_BIND                               = -2118, /* TX source port is not binded */
    ERROR_GN_HDLR_TX_DEST_AREA_DISTANCE_IS_ZERO                      = -2119, /* TX dest area distance setting is zero */
    ERROR_GN_HDLR_TX_SSP_LEN_TOO_LONG                                = -2120, /* The length of SSP is too large */

    ERROR_GN_HDLR_RX_IPC_ESTI_FAIL                                   = -2144, /* Can't estimate GN RX service connection */
    ERROR_GN_HDLR_RX_IPC_IDX_INVALID                                 = -2145, /* Wrong GN RX IPC interface index */
    ERROR_GN_HDLR_RX_TRANSPORT_HDR_TYPE_UNSUPPORTTED                 = -2146, /* Unsupported RX transport header type */
    ERROR_GN_HDLR_RX_PKT_SIZE_INVALID                                = -2147, /* Incorrect RX packet size */
    ERROR_GN_HDLR_RX_BUF_TOO_SHORT                                   = -2148, /* RX packet is larger than inputted buffer */
    ERROR_GN_HDLR_RX_DUPLICATE_BIND                                  = -2149, /* This handler had already been binded before */

    /* dot3 */
    ERROR_DOT3_PARAMS_INVALID                                        = -3072, /* Input parameters are invalid */
    ERROR_DOT3_PACKET_TRUNCATED                                      = -3073, /* Received packet is truncated */
    ERROR_DOT3_ENTRY_NOT_FOUND                                       = -3074, /* Requesting entry is not found */
    ERROR_DOT3_CHANNEL_NOT_FOUND                                     = -3075, /* Requesting channel is not found */
    ERROR_DOT3_PSID_CONFLIT                                          = -3076, /* Requesting PSID has been registered */
    ERROR_DOT3_OUT_OF_RESOURCE                                       = -3077, /* System resourece is full */
    ERROR_DOT3_UNSPEC                                                = -3078, /* Unspecified error */
    ERROR_DOT3_SYS_REINIT                                            = -3079, /* System has been initialized */
    ERROR_DOT3_SYS_NOT_INITED                                        = -3080, /* System has not been initialized */
    ERROR_DOT3_SYS_HANDLE_INVALID                                    = -3081, /* Input handle is invalid */
    ERROR_DOT3_SYS_ERR_INTERNAL                                      = -3082, /* System internal error */
    ERROR_DOT3_SYS_LOSS_OF_IPC                                       = -3083, /* IPC has been closed */
    ERROR_DOT3_SYS_TIMEOUT                                           = -3084, /* Request is timeout */
    ERROR_DOT3_SYS_OUT_OF_MEM                                        = -3085, /* System is out of memory */

    /* its_security */
    ERROR_ITS_SEC_OPEN_UPDATE_DIR_ERROR                              = -4096, /* Open Update Dir fail */
    ERROR_ITS_SEC_OPEN_UPDATE_CA_DIR_ERROR                           = -4097, /* Open Update CA Dir fail */
    ERROR_ITS_SEC_OPEN_UPDATE_EE_DIR_ERROR                           = -4098, /* Open Update EE Dir fail */
    ERROR_ISC_DELETE_UPDATE_FOLDER_ERROR                             = -4099, /* Delete update folder fail */
    ERROR_ISC_MAKE_UPDATE_FOLDER_ERROR                               = -4100, /* Make update folder fail */
    ERROR_ISC_MAKE_UPDATE_CA_FOLDER_ERROR                            = -4101, /* Make update CA folder fail */
    ERROR_ISC_MAKE_UPDATE_EE_FOLDER_ERROR                            = -4102, /* Make update EE folder fail */
    ERROR_ITS_SEC_CERT_FILE_ERROR                                    = -4103, /* Can't access this certificate file or it doesn't exist */
    ERROR_ITS_SEC_STORE_CA_CERT_ERROR                                = -4104, /* Store CA certificate fail */
    ERROR_ITS_SEC_STORE_AT_CERT_ERROR                                = -4105, /* Store AT certificate fail */
    ERROR_ITS_SEC_PRI_KEY_FILE_ERROR                                 = -4106, /* Private key file access fail */
    ERROR_ITS_SEC_BUF_SIZE_NOT_ENOUGH                                = -4107, /* Buf size isn't enough */
    ERROR_ITS_SEC_TBL_NON_EXIST                                      = -4108, /* This table doesn't exist */
    ERROR_ITS_SEC_CMH_EXAHSTED                                       = -4109, /* Dot2 CMH exausted */
    ERROR_ITS_SEC_SET_REPETITIVE_AID_PROFILE_MAPPING                 = -4110, /* This AID has been mapped to a security profile */
    ERROR_ITS_SEC_PROFILE_MAPPING_NON_EXIST                          = -4111, /* This AID associates with profile doesn't exist in mapping table */
    ERROR_ITS_SEC_PROFILE_NON_EXIST                                  = -4112, /* This security profile doesn't exist */
    ERROR_ITS_SEC_PROFILE_NOT_SUPPORT_VERIFY_SIGNED_MSG              = -4113, /* This security profile doesn't support to verify signed message */
    ERROR_ITS_SEC_CERT_NON_EXIST                                     = -4114, /* The corresponding certificate doesn't exist, can't sign/encrypt message */
    ERROR_ITS_SEC_DEL_CERT_NUM_INCONSISTENT                          = -4115, /* Number of deleted certs are not the same as number of whole cert */
    ERROR_ITS_SEC_CMNG_INIT_FAIL                                     = -4116, /* ISC Certificate manager init fail */
    ERROR_ITS_SEC_ENCAP_UNSECURE_ERROR                               = -4117, /* Encap UnsecuredData fail */
    ERROR_ITS_SEC_SIGN_DATA_ERROR                                    = -4118, /* Sign data fail */
    ERROR_ITS_SEC_VERIFY_DATA_ERROR                                  = -4119, /* Verify data fail */
    ERROR_ITS_SEC_INVALID_AID                                        = -4120, /* This AID value is invalid */
    ERROR_ITS_SEC_NO_CERT_INFO                                       = -4121, /* Can't find certificate information */
    ERROR_ITS_SEC_CERT_EXPIRETIME_EXPIRED                            = -4122, /* Certificate expired */
    ERROR_ITS_SEC_CERT_NO_PERMISSION                                 = -4123, /* This certificate don't have enough Service Specific permission(SSP) */
    ERROR_ITS_SEC_INVALID_PROFILE_NAME                               = -4124, /* Invalid profile name */
    ERROR_ITS_SEC_DECAP_INVALID_FMT                                  = -4125, /* Invalid EtsiTs103097 format */
    ERROR_ITS_SEC_DECAP_RAW_BUF_SIZE_NOT_ENOUGH                      = -4126, /* Plaintext buffer size isn't enough */
    ERROR_ITS_SEC_DECAP_TOBESIGN_BUF_SIZE_NOT_ENOUGH                 = -4127, /* Internal ToBeSign buffer size isn't enough */
    ERROR_ITS_SEC_DECAP_NO_CERT                                      = -4128, /* SignerID: certificate, Can't be find in SPDU */
    ERROR_ITS_SEC_DECAP_CERT_CHAIN_EXCEED_SUPPORT_LEN                = -4129, /* Certificate chain exceed length of support */
    ERROR_ITS_SEC_DECAP_PREPROCESS_FAIL                              = -4130, /* Preprocess message fail */

    /* dsrc_security */
    ERROR_DSC_INVALID_PSID                                           = -5120, /* Invalid PSID */
    ERROR_DSC_CERT_MAPPING_TBL_NON_EXIST_CERT                        = -5121, /* Can't find the corresponding certificate in cert mapping table */
    ERROR_DSC_GET_INCORRECT_CERT_FROM_CERT_MAPPING_TBL               = -5122, /* The psid of cert is not consistent with corresponding psid in cert mapping table */
    ERROR_DSC_NUM_OF_DELETED_CERT_INCONSISTENT                       = -5123, /* The number of deleted certificate is inconsistent with certificate in cert mapping table */
    ERROR_DSC_DELETE_STORED_CERT_FAIL                                = -5124, /* Delete those certificates and keys storing in libdot2 fail */
    ERROR_DSC_LOAD_SIGNING_PRI_KEY_FAIL                              = -5125, /* Load ECDSA signging private key fail */
    ERROR_DSC_LOAD_ENCRYPT_PRI_KEY_FAIL                              = -5126, /* Load ECIES encrypt private key fail */
    ERROR_DSC_CMH_EXAHSTED                                           = -5127, /* dot2 CMH has exahsted, Can't acquire cmh more */
    ERROR_DSC_STORE_CA_CERT_ERROR                                    = -5128, /* Store CA certificate error */
    ERROR_DSC_STORE_EE_CERT_AND_KEY_ERROR                            = -5129, /* Store EE certificate and keys error */
    ERROR_DSC_OPEN_UPDATE_CA_DIR_ERROR                               = -5130, /* Open update CA folder fail */
    ERROR_DSC_OPEN_UPDATE_EE_DIR_ERROR                               = -5131, /* Open update EE folder fail */
    ERROR_DSC_SET_REPETITIVE_PSID_PROFILE_MAPPING                    = -5132, /* This PSID has been set to map other security profile */
    ERROR_DSC_SEC_PROFILE_TABLE_SIZE_NOT_ENOUGH                      = -5133, /* The size of security profile table isn't enough */
    ERROR_DSC_FOLDER_PATH_BUF_SIZE_NOT_ENOUGH                        = -5134, /* The size of folder path buffer isn't enough */
    ERROR_DSC_UPDATE_DIR_NOT_EXIST                                   = -5135, /* Update folder dosen't exist */
    ERROR_DSC_CA_CERT_NOT_EXIST                                      = -5136, /* CA certificates don't exist */
    ERROR_DSC_EE_CERT_NOT_EXIST                                      = -5137, /* EE certificates or keys don't exist */
    ERROR_DSC_CERT_NON_EXIST                                         = -5138, /* Certificate doesn't exist */
    ERROR_DSC_DELETE_CMD_BUF_SIZE_NOT_ENOUGH                         = -5139, /* The buffer size of deleted command isn't enough */
    ERROR_DSC_DELETE_UPDATE_FOLDER_ERROR                             = -5140, /* Delete folder for updating certificates fail */
    ERROR_DSC_MAKE_UPDATE_FOLDER_ERROR                               = -5141, /* Make Update folder fail */
    ERROR_DSC_MAKE_UPDATE_CA_FOLDER_ERROR                            = -5142, /* Make Update CA folder fail */
    ERROR_DSC_MAKE_UPDATE_EE_FOLDER_ERROR                            = -5143, /* Make Update EE folder fail */
    ERROR_DSC_OPEN_CERT_FILE_ERROR                                   = -5144, /* Open certificate file fail */
    ERROR_DSC_CERT_MAPPING_TBL_SIZE_NOT_ENOUGH                       = -5145, /* The size of certificate mapping table isn't enough */
    ERROR_DSC_OPEN_SECURITY_CONFIG_FILE_ERROR                        = -5146, /* Open security config file fail */
    ERROR_DSC_PARSE_SECURITY_CONFIG_FILE_ERROR                       = -5147, /* Parse security config file fail */
    ERROR_DSC_GET_CMH_STATUS_ERROR                                   = -5148, /* Get status of CMH from libdot2 fail */
    ERROR_DSC_SECURITY_CONFIG_FILE_PROFILE_TYPE_ERROR                = -5149, /* The profile type of security config file is incorrect */
    ERROR_DSC_SECURITY_SYS_PROFILE_NON_EXIST                         = -5150, /* This security system build-in profile doesn't exist */
    ERROR_DSC_SECURITY_USR_PROFILE_NON_EXIST                         = -5151, /* This security user customzied profile doesn't exist */
    ERROR_DSC_SECURITY_PROFILE_MAPPING_NON_EXIST                     = -5152, /* This security profile mapping doesn't exist */
    ERROR_DSC_SECURITY_PROFILE_NOT_SUPPORT_VERIFY_SIGNED_MSG         = -5153, /* This security profile dosen't support to verify signed message */
    ERROR_DSC_SECURITY_UNKNOWN_PROFILE_TYPE                          = -5154, /* This security profile type is unsuppoted */
    ERROR_DSC_CONFIG_UNKNOWN_PROFILE_TYPE                            = -5155, /* This is a unknown config profile type */
    ERROR_DSC_ENCAP_UNSECURE_MSG_ERROR                               = -5156, /* Encap unsecured msg fail */
    ERROR_DSC_ENCAP_SIGN_MSG_ERROR                                   = -5157, /* Encap signed msg fail */
    ERROR_DSC_CERT_EXPIRETIME_EXPIRED                                = -5158, /* Expiretime of certificate has expired */
    ERROR_DSC_DECAP_INVALID_FMT                                      = -5159, /* Invalid ieee1609.2 format */
    ERROR_DSC_DECAP_RAW_BUF_SIZE_NOT_ENOUGH                          = -5160, /* Plaintext buffer size isn't enough */
    ERROR_DSC_DECAP_TOBESIGN_BUF_SIZE_NOT_ENOUGH                     = -5161, /* Internal ToBeSign buffer size isn't enough */
    ERROR_DSC_DECAP_NO_CERT                                          = -5162, /* SignerID: certificate, Can't be find in SPDU */
    ERROR_DSC_DECAP_CERT_CHAIN_EXCEED_SUPPORT_LEN                    = -5163, /* SignerID certificate chain exceed length of support */
    ERROR_DSC_DECAP_PREPROCESS_FAIL                                  = -5164, /* Preprocess message fail */
    ERROR_DSC_DECAP_VERIFY_DATA_ERROR                                = -5165, /* Verify message fail */
    ERROR_DSC_CERT_MAPPING_TBL_NON_CERT_ITEM                         = -5166, /* None certificate items of assigned psid in Certificate mapping table */
    ERROR_DSC_CERT_NO_PERMISSION                                     = -5167, /* This certificate don't have enough Service Specific permission(SSP) */

    /* v2xcast */
    ERROR_V2XCAST_SERVICE_COMM_CONFIG_INVAILD                        = -6144, /* Communication config is invalid */
    ERROR_V2XCAST_SERVICE_CASTER_LIMIT                               = -6145, /* Caster Input parameters are invalid */
    ERROR_V2XCAST_SERVICE_CASTER_EXISTS                              = -6146, /* Caster exists */
    ERROR_V2XCAST_SERVICE_CASTER_CREATE_FAIL                         = -6147, /* Caster creation fail */
    ERROR_V2XCAST_SERVICE_CASTER_RELEASE_FAIL                        = -6148, /* Caster release fail */
    ERROR_V2XCAST_SERVICE_CASTER_DESTROYED                           = -6149, /* Caster destroyed */
    ERROR_V2XCAST_SERVICE_CASTER_CARRIER_FAIL                        = -6150, /* Caster carrier fail */
    ERROR_V2XCAST_SERVICE_CASTER_UNKNOWN_REQUEST_TYPE                = -6151, /* Caster unknown request type */
    ERROR_V2XCAST_SERVICE_POTI_CREATE_FAIL                           = -6152, /* Poti creation fail */
    ERROR_V2XCAST_SERVICE_POTI_RX_FAIL                               = -6153, /* Poti receiving fail */
    ERROR_V2XCAST_SERVICE_POTI_RELEASE_FAIL                          = -6154, /* Poti release fail */
    ERROR_V2XCAST_SERVICE_POTI_CARRIER_FAIL                          = -6155, /* Poti carrier fail */
    ERROR_V2XCAST_SESSION_CREATE_FAIL                                = -6156, /* Session creation fail */
    ERROR_V2XCAST_SESSION_CONNECT_FAIL                               = -6157, /* Session connection fail */
    ERROR_V2XCAST_SESSION_CLOSE_FAIL                                 = -6158, /* Session close fail */
    ERROR_V2XCAST_SESSION_TRANSMIT_FAIL                              = -6159, /* Session transmition fail */
    ERROR_V2XCAST_SESSION_INIT_RESOURCE_FAIL                         = -6160, /* Session init resource fail */
    ERROR_V2XCAST_SESSION_RECEIVE_FAIL                               = -6161, /* Session receiving fail */
    ERROR_V2XCAST_CONFIG_GBC_BUT_NO_POSITION_INFO                    = -6162, /* Config GBC, but no position info */
    ERROR_V2XCAST_CONFIG_GBC_AREA_DISTANCE_IS_ZERO                   = -6163, /* Config GBC area distance is zero */
    ERROR_V2XCAST_CONFIG_SECURITY_PROFILE_NOT_FOUND                  = -6164, /* Config security prifile is not found */
    ERROR_V2XCAST_CONFIG_SSP_INVAILD_PERMISSIONS                     = -6165, /* Config SSP is invalid permissions */
    ERROR_V2XCAST_CONFIG_SECURITY_FLAG_IS_ON_BUT_GN_CONFIG_SECURITY_NOT_ACTIVATED = -6166, /* Config security flag on, but GN security is off */
    ERROR_V2XCAST_CONFIG_SECURITY_FLAG_IS_OFF_BUT_GN_CONFIG_SECURITY_ACTIVATED = -6167, /* Config security flag off, but GN security is on */
    ERROR_V2XCAST_CONFIG_AID_CERTIFICATE_NOT_FOUND                   = -6168, /* Config AID cert is not found */
    ERROR_V2XCAST_CONFIG_AID_ALREADY_USED                            = -6169, /* Config AID is used */
    ERROR_V2XCAST_CONFIG_AID_INVALID                                 = -6170, /* Config AID is invalid */
    ERROR_V2XCAST_UNKNOWN                                            = -6171, /* Unknown error */

} error_code_e;

#endif // __ERROR_CODE_ENUM_H__

