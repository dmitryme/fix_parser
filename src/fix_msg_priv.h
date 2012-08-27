/* @file   fix_message_priv.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 06:28:42 PM
*/

#ifndef FIX_PARSER_FIX_MESSAGE_PRIV_H
#define FIX_PARSER_FIX_MESSAGE_PRIV_H

#include "fix_types.h"
#include "fix_protocol_descr.h"
#include "fix_page.h"

#include <stdint.h>

struct FIXMessage_
{
   FIXParser* parser;
   FIXMessageDescr* descr;
   FIXGroup* tags;
   FIXPage* pages;
   FIXPage* curr_page;
   FIXGroup* used_groups;
};

void* fix_message_alloc(FIXMessage* msg, uint32_t size);
void* fix_message_realloc(FIXMessage* msg, void* ptr, uint32_t size);
FIXTag* fix_message_set_tag(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, unsigned char const* data, uint32_t len);
FIXTag* fix_message_set_tag_fmt(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, char const* fmt, ...);
FIXGroup* fix_message_get_group(FIXMessage* msg);
void fix_message_free_group(FIXMessage* msg, FIXGroup* grp);

#endif /* FIX_PARSER_FIX_MESSAGE_PRIV_H */