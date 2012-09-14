/* @file   fix_msg_priv.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 06:28:42 PM
*/

#ifndef FIX_PARSER_FIX_MSG_PRIV_H
#define FIX_PARSER_FIX_MSG_PRIV_H

#include "fix_types.h"
#include "fix_protocol_descr.h"
#include "fix_page.h"

#include <stdint.h>

struct FIXMsg_
{
   FIXParser* parser;
   FIXMsgDescr* descr;
   FIXGroup* tags;
   FIXPage* pages;
   FIXPage* curr_page;
   FIXGroup* used_groups;
   uint32_t body_len;
};

void* fix_msg_alloc(FIXMsg* msg, uint32_t size);
void* fix_msg_realloc(FIXMsg* msg, void* ptr, uint32_t size);

FIXTag* fix_msg_get_tag(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);
FIXTag* fix_msg_set_tag(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, unsigned char const* data, uint32_t len);
FIXTag* fix_msg_set_tag_fmt(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, char const* fmt, ...);
int32_t fix_msg_del_tag(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);

FIXGroup* fix_msg_alloc_group(FIXMsg* msg);
void fix_msg_free_group(FIXMsg* msg, FIXGroup* grp);

int32_t fix_tag_to_fix_msg(FIXParser* parser, FIXTag* tag, char delimiter, char** buff, uint32_t* buffLen);
int32_t int32_to_fix_msg(FIXParser* parser, uint32_t tagNum, int32_t val, char delimiter, uint32_t width, char padSym, char** buff, uint32_t* buffLen);
int32_t fix_groups_to_string(FIXMsg* msg, FIXTag* tag, FIXFieldDescr* fdescr, char delimiter, char** buff, uint32_t* buffLen);

#endif /* FIX_PARSER_FIX_MSG_PRIV_H */