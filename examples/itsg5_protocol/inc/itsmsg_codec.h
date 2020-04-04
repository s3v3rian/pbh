/*
 * ITS-G5 message encoder and decoder definitions
 * Copyright (C) Unex Technology Corporation - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#ifndef __ITSMSG_CODEC_H__
#define __ITSMSG_CODEC_H__
#include "asn1helper_if.h"
#include "itsmsg.h"
#include "j2735_df_utils.h"

typedef struct ITSMsgConfig {
    uint8_t *mem_pool_buf; /* (RTOS only) Address of large memory as heap for internal dynamic memory usage  */
    int mem_pool_sz; /* (RTOS only) the size of the above memory pool */
} ITSMsgConfig;

typedef struct ITSMsgCodecErr {
    int bit_pos; /* used for PER decoding */
    int msg_size;
    int msg_len;
    char *msg; /* message */
} ITSMsgCodecErr;

/**
 * do the initialization of ITS message library
 *
 * @param[in]  cfg Address of the configuration of ITS message library.
 *
 * @retval -1  Fail.
 *          0  Success.
 */
extern int itsmsg_init(const ITSMsgConfig *cfg);

/**
 * Allocate the memory of ITSMsg message structure
 *
 * Recursively allocate memory of each fields with list(sequence-of),
 * octec string, bitstring type in the message structure.
 * The allocated size of memory is the max size defined in ITS message ASN.1 definitions.
 * The len of octec and bit string type is filled with the max size.
 * The count of list type is filled with 0.
 * All other memory content are initialized as zero.
 * The memory of field with union type will not be allocated.
 * This is designed to be used with itsmsg_dealloc() in pairs.
 *
 * @param[in]  msg_id The msg type (SPATEM, MAPEM, ...).
 * @param[in]  protocol_ver The version of message aka. protocolVersion in message header.
 * @return It is the address of the memory of message structure.
 * @retval NULL Failed
 */
extern void *itsmsg_prealloc(ITSmsgID msg_id, int protocol_ver);

/**
 * Free the memory of ITS message structure
 *
 * Recursively free memory of each fields with list(sequence-of),
 * octec string, bitstring type in the message structure.
 * The memory of field with union type will not be freed.
 * This is designed to be used with its_msg_prealloc() in pairs.
 *
 * @param[in]  msg_id The msg type (SPATEM, MAPEM, ...)
 * @param[in]  msg The address of the message structure.
 * @retval -1 The msg_id is out of valid range
 *         0 The msg_id is valid
 */
extern int itsmsg_dealloc(ITSmsgID msg_id, void *msg);

/**
 * Encode the pre-defined message structure as UPER format for tansmitting
 *
 * Because user has no way to determine the size of result,
 * the memory is allocated inside the function.
 *
 * @param[in,out]  buf The address for the encoder to store the address of allocated memory
 *                     where the encoded result is stored
 *                     if error, it is NULL and no memory is allocated.
 * @param[in]  msg The message to be encoded
 * @param[in,out]  err The address of the ITSMsgCodecErr structure which is for recording
 *                     the error message if encode fail.
 *                     It could be NULL if it is not necessary.
 * @return It is the size in byte of encoded result, the space which *buf point to.
 * @retval -1 Failed.
 */
extern int itsmsg_encode(uint8_t **buf, ItsPduHeader *msg, ITSMsgCodecErr *err);

/**
 * Decode the UPER format data to pre-defined message structure for processing
 *
 * Because user has no way to determine the type of message after decoding,
 * the memory of decoded message structure is allocated inside the function.
 * The type of decoded message could be gotten from (*msg)->messageId if decode successfully.
 *
 *
 * @param[in,out]  msg The address for the encoder to store the address of allocated memory
 *                     of decoded message structure.
 *                     if error, it is NULL and no memory is allocated.
 * @param[in]  buf The UPER format data to be decoded
 * @param[in]  buf_len The length of buf in bytes
 * @param[in,out]  err The address of the ITSMsgCodecErr structure which is for recording
 *                     the error message if encode fail.
 *                     It could be NULL if it is not necessary.
 * @return The number of consumed bytes.
 * @retval -1 Failed.
 */
extern int itsmsg_decode(ItsPduHeader **msg, const uint8_t *buf, int buf_len, ITSMsgCodecErr *err);

/**
 * Free the memory of message structure
 *
 * Recursively free memory of each fields inside the message structue
 * only when the field is presented which means mandatory field or
 * optional field with presented flag (TRUE).
 * It is designed mainly to free the memory allocated by decoder.
 *
 * WANR: DO NOT use the function to free the memory allocated by itsmsg_peralloc().
 *       It will cause memory leakage.
 *
 * @param[in]  msg The address of message structue to be freed
 *                 and casting it as ItsPduHeader* to be generic
 * @retval -1 The messageID is out of valid or supported range
 *         0  The messageID is valid
 */
extern int itsmsg_free(ItsPduHeader *msg);

/**
 * free the memory of a buffer
 *
 * It just a wrapper of free() in the standard library of C
 * in most of system.
 * To utilize this function is for portability to a RTOS
 * without the standard library of C. It is designed mainly
 * to free the memory allocated by decoding function, itsmsg_encode().
 *
 * @param[in] buf The address of memory pointed by the buffer
 */
extern void itsmsg_buf_free(void *buf);

#endif /* __ITSMSG_CODEC_H__ */
