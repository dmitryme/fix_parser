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

#ifdef __cplusplus
extern "C"
{
#endif

struct FIXMsg_
{
   FIXParser* parser;
   FIXMsgDescr* descr;
   FIXGroup* fields;
   FIXPage* pages;
   FIXPage* curr_page;
   FIXGroup* used_groups;
   uint32_t body_len;
};

void* fix_msg_alloc(FIXMsg* msg, uint32_t size);
void* fix_msg_realloc(FIXMsg* msg, void* ptr, uint32_t size);

FIXField* fix_msg_get_field(FIXMsg* msg, FIXGroup* grp, uint32_t tag);
FIXField* fix_msg_set_field(FIXMsg* msg, FIXGroup* grp, FIXFieldDescr* fdescr, unsigned char const* data, uint32_t len);
int32_t fix_msg_del_field(FIXMsg* msg, FIXGroup* grp, uint32_t tag);

FIXGroup* fix_msg_alloc_group(FIXMsg* msg);
void fix_msg_free_group(FIXMsg* msg, FIXGroup* grp);

#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_MSG_PRIV_H */
