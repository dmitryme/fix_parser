/* @file   fix_msg.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 06:28:42 PM
*/

#ifndef FIX_PARSER_FIX_MSG_H
#define FIX_PARSER_FIX_MSG_H

#include "fix_types.h"

#include <stdint.h>

FIXMsg* fix_msg_create(FIXParser* parser, FIXProtocolVerEnum ver, char const* msgType);
void fix_msg_free(FIXMsg* msg);

FIXTag* fix_msg_get_tag(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);
int fix_msg_del_tag(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);

FIXGroup* fix_msg_add_group(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);
FIXGroup* fix_msg_get_group(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx);
int fix_msg_del_group(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx);

FIXTag* fix_msg_set_tag_string(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, char const* val);
FIXTag* fix_msg_set_tag_long(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, long val);
FIXTag* fix_msg_set_tag_ulong(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, unsigned long val);
FIXTag* fix_msg_set_tag_char(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, char val);
FIXTag* fix_msg_set_tag_float(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, float val);

int fix_msg_get_tag_long(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, long* val);
int fix_msg_get_tag_ulong(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, unsigned long* val);
int fix_msg_get_tag_float(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, float* val);
int fix_msg_get_tag_char(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, char* val);
int fix_msg_get_tag_string(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, char* val, uint32_t len);

int fix_message_to_string(FIXMsg* msg, char delimiter, char* buff, uint32_t buffLen);

#endif /* FIX_PARSER_FIX_MSG_H */