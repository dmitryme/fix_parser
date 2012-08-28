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
};

void* fix_msg_alloc(FIXMsg* msg, uint32_t size);
void* fix_msg_realloc(FIXMsg* msg, void* ptr, uint32_t size);

FIXTag* fix_msg_get_tag(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);
FIXTag* fix_msg_set_tag(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, unsigned char const* data, uint32_t len);
FIXTag* fix_msg_set_tag_fmt(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, char const* fmt, ...);
int fix_msg_del_tag(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);

FIXGroup* fix_msg_alloc_group(FIXMsg* msg);
void fix_msg_free_group(FIXMsg* msg, FIXGroup* grp);

#endif /* FIX_PARSER_FIX_MSG_PRIV_H */