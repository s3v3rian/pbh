/******************************************************************************
*  Copyright 2018, Unex Technology, Co.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Unex Technology, Co. (C) 2019
******************************************************************************/
#ifndef __DOT2_H__
#define __DOT2_H__

/******************************************************************************
Head Block of The File
******************************************************************************/
// Sec 0: Comment block of the file

// Sec 1: Include File
#include <stdint.h>
#include <stdbool.h>

/* 
*   Sec 2: Constant Definitions, Imported Symbols, miscellaneous 
*   Please don't modify them
*/

#define SEC_BSM_PSID 0x20
#define SEC_WSA_PSID 0x87

#define INVALID_PRI_INDEX 0xffffffffUL
#define INVALID_CMH 0xffffffffUL

#define MAX_SPDU_CERT_NUM 1
#define MAX_PSID_NUM 16
/******************************************************************************
Declaration of External Variables & Functions
******************************************************************************/
// Sec 3: declaration of external variable

// Sec 4: declaration of external function prototype

/******************************************************************************
Declaration of data structure
******************************************************************************/
// Sec 5: structure, union, enum, linked list

// Certificate management mode
typedef enum CERT_MAN_MODE {
    CMM_NONE = 0,  // Do nothing at init, just standby
    CMM_SIMPLE_LOAD,  // Simply load certificates from NVM at init

    // not yet defined for other type

    CMM_NUM,
    CMM_UNKNOWN
} dot2_cert_man_mode_t;

// Certificate profile for CMM_SIMPLE_LOAD
typedef struct CERT_PRO_SIMPLE_LOAD {
    char cert_path[128];
    uint32_t load_at_init;
    uint32_t max_num_of_priv_cell;
} dot2_cert_pro_simple_load;

// Certificate name
typedef struct CERT_NAME {
    char filename[64];
} dot2_cert_name_t;

// CHM entry infomation
typedef struct cmh_ent_info {
    uint32_t cmh_idx;
    dot2_cert_name_t cert_name;
    // HA010
    uint32_t psid_num;
    uint32_t psid[MAX_PSID_NUM];  // All PSIDs in appPermission
    uint32_t ssp_len[MAX_PSID_NUM];
    uint8_t ssp[MAX_PSID_NUM][32];  // All SSPs accosiated with PSID in app Permission
    uint32_t expire_time_s;  // Certificate expire time, TAI seconds since 00:00:00 UTC, 2004/1/1
} dot2_cmh_ent_info_t;

typedef enum {
    // General result codes: 0 ~ 49
    DOT2_OK = 0,
    DOT2_FAILURE,
    DOT2_INVALID_KEY_PAIR,
    DOT2_KEYS_DO_NOT_MATCH,
    DOT2_BAD_PARAMETERS,
    DOT2_NOT_SUPPORT,
    DOT2_ASN1_DECODE_FAIL,

    // result codes for dot2_sign: 50 ~ 99
    DOT2_INCORRECT_INPUT = 50,
    DOT2_NO_CERT_PROVIDED,
    DOT2_NO_PUBLIC_KEY_PROVIDED,
    DOT2_NOT_ENOUGH_INFO_TO_CONTRUCT_CERT_CHAIN,
    DOT2_NO_TRUST_ANCHOR,
    DOT2_CERT_CHAIN_TOO_LONG_FOR_IMPLEMENTATION,
    DOT2_NOT_CRYPTOGRAPHICALLY_VALID,
    DOT2_UNKNOWN_CRYPTOGRAPHICALLY_VALID,
    DOT2_INCONSISTENT_PERMISSIONS_IN_CERT_CHIAN,
    DOT2_REVOKED,
    DOT2_DUBIOUS,
    DOT2_UNSUPPORTED_CRITICAL_INFO_FIELDS,
    DOT2_INVALID_ENCODING,
    DOT2_CURRENT_TIME_BEFORE_CERT_VALIDITY_PERIOD,
    DOT2_CURRENT_TIME_AFTER_CERT_VALIDITY_PERIOD,
    DOT2_EXPIRY_TIME_BEFORE_CERT_VALIDITY_PERIOD,
    DOT2_EXPIRY_TIME_AFTER_CERT_VALIDITY_PERIOD,
    DOT2_INVALID_GENERATION_LOCATION,
    DOT2_INCONSISTENT_PERMISSIONS_IN_CERT,
    DOT2_INCORRECT_REQUESTED_CERT_CHAIN_LENGTH_FOR_SECURITY_PROFILE,
    DOT2_INCORRECT_REQUESTED_CERT_CHAIN_LENGTH_FOR_IMPLEMENTATION,

    // result codes for dot2_encrypt: 100 ~ 149
    DOT2_FAIL_ON_SOME_CERTS = 100,
    DOT2_FAIL_ON_ALL_CERTS,
    DOT2_NO_ENCRYPT_CERT_PROVIDED,

    // result codes for dot2_sec_data_preprocessing: 150 ~ 199
    DOT2_UNKNOWN_CERT = 150,
    DOT2_INCONSISTENT_PSID,

    // result codes for dot2_sec_data_verification: 200 ~ 249
    DOT2_INCONSISTENT_INPUT_PARAMETERS = 200,
    DOT2_PARSING_INVALID_INPUT,
    DOT2_PARSING_UNSUPPORTED_CRITICAL_INFO_FIELDS,
    DOT2_PARSING_CERT_NOT_FOUND,
    DOT2_PARSING_GENERATION_TIME_NOT_AVAILABLE,
    DOT2_PARSING_GENERATION_LOCATION_NOT_AVAILABLE,
    DOT2_CERT_CHAIN_NOT_ENOUGH_INFO_TO_CONSTRUCT_CHAIN,
    DOT2_CERT_CHAIN_CHAIN_ENDED_AT_UNTRUSTED_ROOT,
    DOT2_CERT_CHAIN_CHAIN_WAS_TOO_LONG_FOR_IMPLEMENTATION,
    DOT2_CERT_CHAIN_CERT_REVOKED,
    DOT2_CERT_CHAIN_OVERDUE_CRL,
    DOT2_CERT_CHAIN_INCONSISTENT_EXPIRY_TIMES,
    DOT2_CERT_CHAIN_INCONSISTENT_START_TIMES,
    DOT2_CERT_CHAIN_INCONSISTENT_CHAIN_PERMISSIONS,
    DOT2_CRYPTO_VERIFICATION_FAILURE,
    DOT2_CONSISTENCY_FUTURE_CERT_AT_GENERATION_TIME,
    DOT2_CONSISTENCY_EXPIRED_CERT_AT_GENERATION_TIME,
    DOT2_CONSISTENCY_EXPIRY_DATE_TOO_EARLY,
    DOT2_CONSISTENCY_EXPIRY_DATE_TOO_LATE,
    DOT2_CONSISTENCY_GENERATION_LOCATION_OUTSIDE_VALIDITY_REGION,
    DOT2_CONSISTENCY_NO_GENERATION_LOCATION,
    DOT2_CONSISTENCY_UNAUTHORIZED_PSID,
    DOT2_INTERNAL_CONSISTENCY_EXPIRY_TIME_BEFORE_GENERATION_TIME,
    DOT2_INTERNAL_CONSISTENCY_EXDATAHASH_DOESNT_MATCH,
    DOT2_INTERNAL_CONSISTENCY_NO_EXDATAHASH_PROVIDED,
    DOT2_INTERNAL_CONSISTENCY_NO_EXDATAHASH_PRESENT,
    DOT2_LOCAL_CONSISTENCY_PSIDS_DONT_MATCH,
    DOT2_LOCAL_CONSISTENCY_CHAIN_WAS_TOO_LONG_FOR_SDEE,
    DOT2_RELEVANCE_GENERATION_TIME_TOO_FAR_IN_PAST,
    DOT2_RELEVANCE_GENERATION_TIME_TOO_FAR_IN_FUTURE,
    DOT2_RELEVANCE_EXPIRY_TIME_IN_PAST,
    DOT2_RELEVANCE_GENERATION_LOCATION_TOO_DISTANT,
    DOT2_RELEVANCE_REPLAYED_SPDU,
    DOT2_RELEVANCE_CERT_EXPIRED,

    // result codes for dot2_decrypt: 250 ~ 299
    DOT2_NO_DECRYPTION_KEY_AVAILABLE = 250,
    DOT2_COULDNT_DECRYPT_KEY,
    DOT2_COULDNT_DECRYPT_DATA,
    DOT2_INVALID_FORM_FOR_PLAINTEXT,

    // result codes for ssme_certificate_info: 300 ~ 349
    DOT2_CERT_NOT_FOUND = 300,
    DOT2_MULTIPLE_CERTS_IDENTIFIED,
    DOT2_NOT_YET_VERIFIED,
    DOT2_VERIFIED_AND_TRUSTED,
    // result codes for ssme_add_trust_anchor
    DOT2_CERT_DID_NOT_VERIFY,
    // result codes for ssme_verify_certificate
    DOT2_VERIFIED,
    // result codes for ssme_sec_replay_detection
    DOT2_REPLAY,
    DOT2_NOT_REPLAY,

    // result code for CMH: 350 ~ 399
    DOT_CMH_CACHE_FULL = 350,
    DOT_CMH_MEM_ALLOCATE_FAIL,

    // result code for Certificate Manager: 400 ~ 449
    DOT2_CERT_MAN_WRONG_STATE = 400,
    DOT2_CERT_MAN_NOT_SUPPORT,
    DOT2_CERT_MAN_DIR_ERROR,
    DOT2_CERT_MAN_FILE_ERROR,
    DOT2_CERT_MAN_MEM_ALLOCATE_FAIL,

    // others: 1000 ~
    DOT2_MSG_TOO_LONG = 1000,
    DOT2_TOBESIGNED_MSG_TOO_LONG,
    DOT2_GPS_LOCATION_NOT_AVAILABLE,
    DOT2_INVALID_SIGNER_FOR_SIGNED_MSG,
    DOT2_UNSUPPORTED_CERT_CHAIN_LEN,
    DOT2_PARSED_CERT_LENGTH_NOT_MATCH,
    DOT2_UNSUPPORTED_FIELDS,
    DOT2_NO_SPACE_FOR_PUB_CERT,
    DOT2_KICK_OFF_CERT_FOR_PUB_CERT,
    DOT2_SYMMETRIC_KEY_NOT_FOUND,
    DOT2_POINT_UNCOMPRESSION_FAILED,
    DOT2_POINT_COMPRESSION_FAILED,
    DOT2_AES_CCM_ENCRYPT_FAILED,
    DOT2_TOO_MANY_RECIPIENTS,
    DOT2_UNSUPPORT_RECIPIENT_TYPE,


    RESERVED_RET_CODE
} dot2_return_code_t;

typedef enum {
    NONE_EC_ALGORITHM = 0,  // to prevent cert_t & cmh_t initilize misunderstand
    ECDSA_NISTP256_WITH_SHA256,
    ECDSA_BRAINPOOLP256R1_WITH_SHA256,
    ECIES_NISTP256,
    ECIES_BRAINPOOLP256R1,
    RESERVED_EC_ALGORITHM  // for checking validity
} dot2_ec_algorithms_t;

typedef enum {
    AES_128_CCM = 0,
    RESERVED_SYM_ALGORITHM  // for checking validity
} dot2_sym_algorithms_t;

typedef enum {
    ISOLATED_KEY = 0,
    RECONSTRUCT_SEED = 1,
} private_key_type_t;

typedef enum {
    IEEE_1609_DOT2_DATA = 0,
    RAW
} data_type_t;

typedef enum {
    UNSECURE,
    SIGNED,
    ENCRYPTED,
    SIGNEDCERTIFICATEREQUEST,
    RESERVED_DATA_TYPE
} dot2_content_type_t;

typedef enum {
    DIGEST = 0,
    CERTIFICATE = 1,
    SELF = 2,
    RESERVED_SIGNER_TYPE
} dot2_signer_type_t;

typedef enum {
    X_ONLY = 0,
    COMPRESSED = 1,
    UNCOMPRESSED = 2,
    RESERVED_EC_POINT_CHOICE
} dot2_ec_point_choice_t;

typedef enum {
    END_ENTITY = 0,
    CA_CERT = 1,  // CA Certificate
    RESERVED_CERT_TYPE
} dot2_cert_type_t;

// basic types
typedef struct {
    int latitude;
    int longitude;
    unsigned short elevation;
} _3dloc;

typedef struct {
    dot2_ec_algorithms_t alg;
    unsigned char r[65];
    unsigned char s[32];
} signature_t;


typedef struct {
    unsigned int cmh;
    unsigned int data_len;
    unsigned char* data;
    data_type_t data_type;  // RAW or Ieee1609Dot2Data
    unsigned char ext_data_hash[32];
    unsigned char ext_data_hash_alg;  // should be sha256
    unsigned int psid;
    bool set_gen_time;  // true or false
    bool set_gen_loc;  // true or false
    unsigned int exp_time_ms;  // time duration in ms
    dot2_signer_type_t signer_id_type;  // cert, digest, self
    char num_of_signer_id_cert;  // 1~256, 0 for MAX
    unsigned char max_num_of_signer_id_cert;
    dot2_ec_point_choice_t sign_with_fast_ver;  // y/w uncompressed, y/w compressed, no
    uint32_t signed_data_len; /* in: max length of signed_data; */
    uint8_t* signed_data; /* out: actual length of signed_data; */
} sec_sign_t;

typedef struct {
    unsigned int psid;
    dot2_content_type_t content_type;
    unsigned int tbs_signed_data_len;
    unsigned char* tbs_signed_data;
    unsigned char external_data_hash[32];
    unsigned char hash_alg;
    unsigned char max_num_of_signer_id_cert;
    unsigned char data_hash_for_replay_check[32];
    bool replay_check;  // true or false
    unsigned long long gen_time_us;  // provided if header not contain gentime
    bool gen_time_in_past_check;  // if true, reject too-old SPDU
    unsigned int validity_period;  // provided if gentimeinpast = true
    bool gen_time_in_future_check;  // if true, reject future SPDU
    unsigned int future_data_period;  // provided if gentimeinfuture = true
    bool expiry_check;  // if true, reject expired SPDU
    unsigned long long expiry_time_us;  // provided if expirycheck = true
    bool gen_loc_in_cert_check;  // check genloc is inside the region of cert
    bool gen_loc_dis_check;  // check genloc is inside the relevance range
    double validity_dis;  // validity range between recp and genloc (m)
    _3dloc gen_loc;  // provided if header not contain genloc
    unsigned int crl_tolerance;
    bool cert_exp_check;  // if true, reject if any one of cert in chain is expired

    unsigned char signerid8[8];
    signature_t sig;
} sec_sec_data_verification_t;

typedef struct {
    unsigned char hashId[32];
    bool is_exist_encrypt_pub_key;
} spdu_cert_info_t;

typedef struct {
    unsigned int data_len;
    unsigned char* data;
    dot2_content_type_t content_type;
    unsigned int psid;
    unsigned int ssp_len;
    unsigned char ssp[32];
    unsigned char assurance_level;
    unsigned int next_crl_time;

    spdu_cert_info_t spdu_cert_inf[MAX_SPDU_CERT_NUM];
    unsigned int spdu_cert_num;
    unsigned int raw_data_len;  // in: max array size; out: actual data size
    unsigned char* raw_data;
    sec_sec_data_verification_t data_info;  // put extracted info here
} sec_sec_data_preprocessing_t;

typedef struct {
    unsigned int data_len;
    unsigned char* data;
    data_type_t data_type;  // RAW or Ieee1609Dot2Data
    unsigned int scmh;
    unsigned int recp_cert_num;
    unsigned char* recp_cert_id_list;  // list of recp cert id (hashid8)
    unsigned char resp_enc_key[8];
    dot2_ec_point_choice_t ecpoint_format;

    unsigned int enc_data_len;  // in: max length of enc_data;
    // out: actual length of enc_data;
    unsigned char* enc_data;
    unsigned int fail_cert_num;
    unsigned char* fail_cert_id_list;  // list of failed cert id (hashid8)
} sec_encrypt_t;


typedef struct {
    unsigned int data_len;
    unsigned char* data;
    unsigned int decrypted_data_len;  // in: max length of decrypted_data;
    /* out: actual length of decrypted_data; */
    unsigned char* decrypted_data;
} sec_decrypt_t;

typedef struct {
    unsigned int i_cmh;
    unsigned char i_sign_flag;
    unsigned char i_enc_flag;
    unsigned int i_enc_cert_num;
    unsigned char i_enc_cert_hashId[MAX_SPDU_CERT_NUM][32];
    unsigned int i_msg_len;
    unsigned char* i_msg;
    unsigned int i_lifetime_ms;  // units: ms
    unsigned int o_msg_len;
    unsigned char* o_msg;
    unsigned int fail_cert_num;
    unsigned char* fail_cert_id_list;
} dot2_encode_msg_t;

typedef struct {
    unsigned char i_verify_flag;
    unsigned int i_msg_len;
    unsigned char* i_msg;
    unsigned int o_msg_len;
    unsigned char* o_msg;
    unsigned long long o_generation_time_us;
    unsigned long long o_expiration_time_us;
    unsigned int o_next_crl_time;
    spdu_cert_info_t spdu_cert_inf[MAX_SPDU_CERT_NUM];
    unsigned int spdu_cert_num;
} dot2_decode_msg_t;


/******************************************************************************
Declaration of Global Variables & Functions
******************************************************************************/
// Sec 6: declaration of global variable

// Sec 7: declaration of function prototype

// Init & exit -------------------------------------------------------
int dot2_init(dot2_cert_man_mode_t i_cert_man_mode,
              void* i_cert_profile);

int dot2_exit(void);


// Cryptomaterial Management -----------------------------------------
int dot2_cmh_acquire(uint32_t* io_cmh, uint8_t i_force);

int dot2_cmh_key_gen(uint32_t i_cmh,
                     dot2_ec_algorithms_t i_alg,
                     private_key_type_t i_key_type,
                     uint8_t* o_pub_key,
                     dot2_cert_name_t* i_ptr_key_name);

int dot2_cmh_key_store(uint32_t i_cmh,
                       dot2_ec_algorithms_t i_alg,
                       private_key_type_t key_type,
                       uint8_t* i_pub_key,
                       uint8_t* i_pri_key,
                       dot2_cert_name_t* i_ptr_key_name);

int dot2_cmh_key_load(dot2_cert_name_t* i_ptr_key_name,
                      uint32_t* o_ptr_chm);

int dot2_cmh_cert_store(uint32_t i_cmh,
                        uint32_t i_cert_len,
                        uint8_t* i_cert,
                        uint8_t* i_pri_key_material,
                        dot2_cert_name_t* i_ptr_cert_name);

int dot2_cmh_cert_and_key_store(uint32_t i_cmh,
                                uint32_t i_cert_len,
                                uint8_t* i_cert,
                                uint8_t* i_ecdsa_pri_key,
                                uint8_t* i_ecies_pri_key,
                                dot2_cert_name_t* i_ptr_cert_name);

int dot2_cmh_del(uint32_t i_cmh);

int dot2_cmh_clear(uint32_t i_cmh);

int dot2_cmh_all_cert_and_key_delete(void);

int dot2_cmh_cache_status_get(uint32_t* o_ptr_cache_size,
                              uint32_t* o_ptr_used_num,
                              dot2_cmh_ent_info_t** o_pptr_cmh_ent_info,
                              uint32_t* o_ptr_ca_cache_size,
                              uint32_t* o_ptr_ca_used_num,
                              dot2_cmh_ent_info_t** o_pptr_ca_ent_info);

// HA010
int dot2_cmh_ent_info_get(uint32_t i_cmh,
                          dot2_cmh_ent_info_t* o_ptr_cmh_ent_info);

int dot2_cert_file_status_get(uint32_t* o_ptr_cert_num,
                              dot2_cert_name_t** o_pptr_cert_list,
                              uint32_t* o_ptr_ca_num,
                              dot2_cert_name_t** o_pptr_ca_list);

int dot2_cmh_cert_load(dot2_cert_name_t* i_ptr_cert_name,
                       uint32_t* o_ptr_chm);

int dot2_cmh_cert_hashid_get(uint32_t i_cmh,
                             uint8_t* o_cert_id);

// Symmetric Cryptomaterial Management -------------------------------
int dot2_scmh_new(dot2_sym_algorithms_t i_alg,
                  uint8_t i_gen_flag,
                  uint8_t* i_sym_key,
                  uint32_t* o_scmh);

int dot2_scmh_hashid8(uint32_t i_scmh, uint8_t* o_hashid8);

int dot2_scmh_del(uint32_t i_scmh);


// CA Certificate ----------------------------------------------------
int dot2_cmh_ca_cert_store(uint32_t i_cert_len,
                           uint8_t* i_cert,
                           uint32_t* o_cmh,
                           dot2_cert_name_t* i_ptr_cert_name);

int dot2_cmh_ca_del(uint32_t i_cmh);

int dot2_cmh_ca_clear(uint32_t i_cmh);

int dot2_cmh_ca_cert_load(dot2_cert_name_t* i_ptr_cert_name,
                          uint32_t* o_ptr_chm);


// Security Message Service ------------------------------------------
int dot2_sign(sec_sign_t* sign_info);

int dot2_sec_data_preprocessing(sec_sec_data_preprocessing_t* sec_data);

int dot2_sec_data_verification(sec_sec_data_verification_t* sec_data);

int dot2_encrypt(sec_encrypt_t* encrypt_info);

int dot2_decrypt(sec_decrypt_t* decrypt_info);

int dot2_encode_msg(uint32_t i_psid, dot2_encode_msg_t* para);

int dot2_decode_msg(uint32_t i_psid, dot2_decode_msg_t* para);


/******************************************************************************
Declaration of Global Variables & Functions
******************************************************************************/
// Sec 6: declaration of global variable

// Sec 7: declaration of function prototype

/******************************************************************************
Declaration of static Global Variables & Functions
******************************************************************************/
// Sec 8: declaration of static global variable

// Sec 9: declaration of static global function prototype

/******************************************************************************
C Functions
******************************************************************************/
// Sec 10: C Functions


#endif /* __DOT2_API_H__ */
