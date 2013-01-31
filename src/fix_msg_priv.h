/**
 * @file   fix_msg_priv.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 06:28:42 PM
 */

#ifndef FIX_PARSER_FIX_MSG_PRIV_H
#define FIX_PARSER_FIX_MSG_PRIV_H

#include "fix_types.h"
#include "fix_protocol_descr.h"
#include "fix_page.h"

#include <stdint.h>

#pragma pack(push, 1)
#pragma pack(1)

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * FIX message
 */
struct FIXMsg_
{
   FIXParser* parser;         ///< pointer to parser, who holds this message
   FIXMsgDescr const* descr;  ///< FIX message description
   FIXGroup* fields;          ///< message FIX fields
   FIXPage* pages;            ///< allocated pages with FIX field data
   FIXPage* curr_page;        ///< current memory page
   FIXGroup* used_groups;     ///< used groups by this message
   uint32_t body_len;         ///< entire body len, if message converted to FIX data
};

/**
 * allocate data for this message
 * @param[in] msg - pointer to message
 * @param[in] size - size of data being allocated
 * @return pointer to allocated space
 */
void* fix_msg_alloc(FIXMsg* msg, uint32_t size);

/**
 * realloc previous allocated space
 * @param[in] msg - msg with allocated space
 * @param[in] ptr - pointer to data previously allocated
 * @param[in] size - new size of allocated space. If new size greater, new space will be allocated, else no new space allocated
 * @return pointer to reallocated space
 */
void* fix_msg_realloc(FIXMsg* msg, void* ptr, uint32_t size);

/**
 * return FIX field by tag num
 * @param[in] msg - FIX message with required FIX field
 * @param[in] grp - pointer to FIX group, if tag is a part of FIX groups, else must be NULL
 * @param[in] tag - FIX field tag num
 * @return required FIX field or NULL in case of error
 */
FIXField* fix_msg_get_field(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag);

/**
 * set FIX field value
 * @param[in] msg - FIX msg with being changed field
 * @param[in] grp - FIX group, if FIX field is a part of it, else must be NULL
 * @param[in] fdescr - FIX field description
 * @param[in] data - new FIX field value
 * @param[in] len - length of data
 */
FIXField* fix_msg_set_field(FIXMsg* msg, FIXGroup* grp, FIXFieldDescr const* fdescr, unsigned char const* data, uint32_t len);

/**
 * create new FIX group
 * @param[in] msg - FIX message
 * @return new FIX group
 */
FIXGroup* fix_msg_alloc_group(FIXMsg* msg);

/**
 * destroy FIX group
 * @param[in] msg - FIX message
 * @param[in] grp - pointer to deallocated group
 */
void fix_msg_free_group(FIXMsg* msg, FIXGroup* grp);

/**
 * converts FIX group to string
 * @param[in] msg - FIX message with converted FIX group
 * @param[in] field - FIX field with group data
 * @param[in] fdescr - FIX field description
 * @param[in] delimiter - FIX field SOH
 * @param[out] buff - space with converted data
 * @param[out] buffLen - size of converted data
 * @param[out] crc - ajustment of crc. Can be differ from 0, if delimiter is not a FIX_SOH
 * @return FIX_SUCCESS - ok, FIX_FAILED - error
 */
FIXErrCode fix_groups_to_string(FIXMsg* msg, FIXField const* field, FIXFieldDescr const* fdescr, char delimiter, char** buff, uint32_t* buffLen, int32_t* crc);

/**
 * convert numeric value to string
 * @param[in] parser - FIX parser
 * @param[in] tag - FIX field tag value
 * @param[in] val - converted value
 * @param[in] delimiter - FIX field delimiter
 * @param[in] width - value width
 * @param[in] padSym - char for padding value width. E.g. tag = 35, width = 5, padSym = '0' and value is 10, so converted data is '35=00010'
 * @param[out] buff - buffer with converted data
 * @param[out] buffLen - size of converted data
 * @return FIX_SUCCESS - ok, FIX_FAILED - error
 */
FIXErrCode int32_to_fix_msg(FIXParser* parser, FIXTagNum tag, int32_t val, char delimiter, uint32_t width, char padSym, char** buff, uint32_t* buffLen);

/**
 * convert FIX field to string
 * @param[in] parser - FIX parser
 * @param[in] field - FIX field being converted
 * @param[in] delimiter - FIX field SOH
 * @param[out] buff - buffer with converted data
 * @param[out] buffLen - size of converted data
 * @return FIX_SUCCESS - ok, FIX_FAILED - error
 */
FIXErrCode fix_field_to_fix_msg(FIXParser* parser, FIXField const* field, char delimiter, char** buff, uint32_t* buffLen);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif /* FIX_PARSER_FIX_MSG_PRIV_H */
