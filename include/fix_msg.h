/* @file   fix_msg.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 06:28:42 PM
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

FIX_PARSER_API FIXMsg* fix_msg_create(FIXParser* parser, char const* msgType);
FIX_PARSER_API void fix_msg_free(FIXMsg* msg);

FIX_PARSER_API FIXGroup* fix_msg_add_group(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum);
FIX_PARSER_API FIXGroup* fix_msg_get_group(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, uint32_t grpIdx);
FIX_PARSER_API FIXErrCode fix_msg_del_group(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, uint32_t grpIdx);

FIX_PARSER_API FIXErrCode fix_msg_set_string(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char const* val);
FIX_PARSER_API FIXErrCode fix_msg_set_int32(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, int32_t val);
FIX_PARSER_API FIXErrCode fix_msg_set_int64(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, int64_t val);
FIX_PARSER_API FIXErrCode fix_msg_set_char(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char val);
FIX_PARSER_API FIXErrCode fix_msg_set_double(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, double val);

FIX_PARSER_API FIXErrCode fix_msg_get_int32(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, int32_t* val);
FIX_PARSER_API FIXErrCode fix_msg_get_int64(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, int64_t* val);
FIX_PARSER_API FIXErrCode fix_msg_get_double(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, double* val);
FIX_PARSER_API FIXErrCode fix_msg_get_char(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char* val);
FIX_PARSER_API FIXErrCode fix_msg_get_string(FIXMsg* msg, FIXGroup* grp, FIXTagNum tagNum, char const** val, uint32_t* len);

FIX_PARSER_API FIXErrCode fix_msg_to_fix(FIXMsg* msg, char delimiter, char* buff, uint32_t buffLen, uint32_t* reqBuffLen);

#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_MSG_H */
