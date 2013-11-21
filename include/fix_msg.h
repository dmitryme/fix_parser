/**
 * @file   fix_msg.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 06:28:42 PM
 */

#ifndef FIX_PARSER_FIX_MSG_H
#define FIX_PARSER_FIX_MSG_H

#include "fix_types.h"
#include "fix_parser_dll.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * create new FIX message
 * @param[in] parser - instance of parser
 * @param[in] msgType - type of message (e.g. "A", "D", "AE", etc.)
 * @param[out] error - error description
 * @return pointer to created message, is NULL see fix_parser_get_error_code(parser) for error
 * @note all allocated messages must be destroyed with fix_msg_free
 */
FIX_PARSER_API FIXMsg* fix_msg_create(FIXParser* parser, char const* msgType, FIXError** error);
/**
 * free allocated message
 * @param[in] msg - message, which should be destroyed.
 */
FIX_PARSER_API void fix_msg_free(FIXMsg* msg);

/**
 * return message type. E.g. "A", "8", "D", etc
 * @param[in] msg - fix message
 * @return message type
 */
FIX_PARSER_API char const* fix_msg_get_type(FIXMsg const* msg);

/**
 * return message name. E.g. "Logon", "ExecutionReport", "NewOrderSingle", etc
 * @param[in] msg - fix message
 * @return message name
 */
FIX_PARSER_API char const* fix_msg_get_name(FIXMsg const* msg);

/**
 * add new FIX group to tag
 * @param[in] msg - FIX message
 * @param[in] grp - parent group. Can be NULL if group added to message tag, or not null is new nested group should be
 * created
 * @param[in] tagNum - field tag in which new group will be added
 * @param[out] error - error description
 * @return pointer to new group, if NULL, see fix_parser_get_error_code(parser)
 */
FIX_PARSER_API FIXGroup* fix_msg_add_group(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, FIXError** error);
/**
 * get group by zero-based index
 * @param[in] msg - message with requested group
 * @param[in] grp - parent group, in case in nested group requested. Can be NULL.
 * @param[in] tagNum - field tag with requested group
 * @param[in] grpIdx - zero-based index of requested group
 * @param[out] error - error description
 * @return requested group, if NULL see fix_parser_get_error_code(parser) for details
 */
FIX_PARSER_API FIXGroup* fix_msg_get_group(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, uint32_t grpIdx, FIXError** error);
/**
 * delete group by zero-based index
 * @param[in] msg - FIX message
 * @param[in] grp - parent group, if deleted group is a nested one, else can be NULL.
 * @param[in] tagNum - field tag with deleted group
 * @param[in] grpIdx - zero-based index of deleted group
 * @param[out] error - error description
 * @return FIX_SUCCESS - is OK, FIX_FAILED - something goes wrong. See fix_parser_get_error_code(parser) for details
 * */
FIX_PARSER_API FIXErrCode fix_msg_del_group(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, uint32_t grpIdx, FIXError** error);

/**
 * set tag with string value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[in] val - string value
 * @param[in] len - string length
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK, FIX_FAILED - not set. See fix_parser_get_error_code(parser) for details
 */
FIX_PARSER_API FIXErrCode fix_msg_set_string_len(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char const* val, uint32_t len,
      FIXError** error);

/**
 * set tag with string value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[in] val - string value (ends with zero)
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK, FIX_FAILED - not set. See fix_parser_get_error_code(parser) for details
 */
FIX_PARSER_API FIXErrCode fix_msg_set_string(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char const* val, FIXError** error);

/**
 * set tag with 32-bit numeric value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[in] val - 32-bit numeric value
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK, FIX_FAILED - not set. See fix_parser_get_error_code(parser) for details
 */
FIX_PARSER_API FIXErrCode fix_msg_set_int32(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, int32_t val, FIXError** error);

/**
 * set tag with 64-bit numeric value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[in] val - 64-bit numeric value
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK, FIX_FAILED - not set. See fix_parser_get_error_code(parser) for details
 */
FIX_PARSER_API FIXErrCode fix_msg_set_int64(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, int64_t val, FIXError** error);

/**
 * set tag with single char value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[in] val - char value
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK, FIX_FAILED - not set. See fix_parser_get_error_code(parser) for details
 */
FIX_PARSER_API FIXErrCode fix_msg_set_char(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char val, FIXError** error);

/**
 * set tag with double value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[in] val - double value
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK, FIX_FAILED - not set. See fix_parser_get_error_code(parser) for details
 */
FIX_PARSER_API FIXErrCode fix_msg_set_double(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, double val, FIXError** error);

/**
 * set tag with data value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[in] data - data value
 * @param[in] dataLen - data value length
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK, FIX_FAILED - not set. See fix_parser_get_error_code(parser) for details
 */
FIX_PARSER_API FIXErrCode fix_msg_set_data(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char const* data, uint32_t dataLen, FIXError** error);

/**
 * get tag 32-bit value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[out] val - requested value. Pointer to sizeof(int32_t) allocated space required.
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK
 *         FIX_NO_FIELD - field not found
 *         FIX_FAILED - error description
 */
FIX_PARSER_API FIXErrCode fix_msg_get_int32(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, int32_t* val, FIXError** error);

/**
 * get tag 64-bit value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[out] val - requested value. Pointer to sizeof(int64_t) allocated space required.
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK
 *         FIX_NO_FIELD - field not found
 *         FIX_FAILED - error description
 */
FIX_PARSER_API FIXErrCode fix_msg_get_int64(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, int64_t* val, FIXError** error);

/**
 * get tag double value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[out] val - requested value. Pointer to sizeof(double) allocated space required.
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK
 *         FIX_NO_FIELD - field not found
 *         FIX_FAILED - error description
 */
FIX_PARSER_API FIXErrCode fix_msg_get_double(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, double* val, FIXError** error);

/**
 * get tag char value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[out] val - requested value. Pointer to sizeof(char) allocated space required.
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK
 *         FIX_NO_FIELD - field not found
 *         FIX_FAILED - error description
 */
FIX_PARSER_API FIXErrCode fix_msg_get_char(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char* val, FIXError** error);

/**
 * get tag string value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[out] val - pointer to requested value
 * @param[out] len - length of requested value
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK
 *         FIX_NO_FIELD - field not found
 *         FIX_FAILED - not get. See fix_parser_get_error_code(parser) for details
 */
FIX_PARSER_API FIXErrCode fix_msg_get_string(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char const** val, uint32_t* len, FIXError** error);

/**
 * get tag data value
 * @param[in] msg - FIX message
 * @param[in] grp - non NULL group, if tag is a part of group, else must be NULL
 * @param[in] tagNum - field tag number
 * @param[out] val - pointer to requested value
 * @param[out] len - length of requested value
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK
 *         FIX_NO_FIELD - field not found
 *         FIX_FAILED - error description
 */
FIX_PARSER_API FIXErrCode fix_msg_get_data(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char const** val, uint32_t* len, FIXError** error);

/**
 * delete field from message
 * @param[in] msg - message with tag, which will be deleted
 * @param[in] grp - not NULL group pointer, if tag is a part of group, else must be NULL
 * @param[in] tag - field tag, which should be removed
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK,
 *         FIX_NO_FIELD - no such field, nothing to delete
 *         FIX_FAILED - error description
 */
FIX_PARSER_API FIXErrCode fix_msg_del_field(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, FIXError** error);

/**
 * convert FIX message to string
 * @param[in] msg - message to be converted
 * @param[in] delimiter - FIX field delimter char
 * @param[out] buff - buffer with converted message
 * @param[out] buffLen - length of output buffer
 * @param[out] reqBuffLen - if buff length too small, reqBuffLen returns length of needed space
 * @param[out] error - error description
 * @return FIX_SUCCESS - OK
 *         FIX_ERROR_NO_NORE_SPACE - see reqBuffLen for required space
 *         FIX_FAILED - error description
 */
FIX_PARSER_API FIXErrCode fix_msg_to_str(FIXMsg* msg, char delimiter, char* buff, uint32_t buffLen, uint32_t* reqBuffLen, FIXError** error);

#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_MSG_H */
