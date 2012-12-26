/* @file   fix_parser_priv.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:54:30 AM
*/

#ifndef FIX_PARSER_FIX_PARSER_PRIV_H
#define FIX_PARSER_FIX_PARSER_PRIV_H

#include "fix_types.h"
#include "fix_protocol_descr.h"
#include "fix_page.h"
#include "fix_field.h"
#include "fix_error_priv.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SOH 0x01

struct FIXParser_
{
   FIXProtocolDescr* protocol;
   FIXError error;
   FIXParserAttrs attrs;
   int32_t  flags;
   FIXPage* page;
   uint32_t used_pages;
   FIXGroup* group;
   uint32_t used_groups;
};

FIXPage* fix_parser_alloc_page(FIXParser* parser, uint32_t pageSize);
FIXPage* fix_parser_free_page(FIXParser* parser, FIXPage* page);
FIXGroup* fix_parser_alloc_group(FIXParser* parser);
FIXGroup* fix_parser_free_group(FIXParser* parser, FIXGroup* group);
int64_t fix_parser_parse_mandatory_field(
      FIXParser* parser, char const* data, uint32_t len, char delimiter, char const** dbegin, char const** dend);
int64_t fix_parser_parse_field(
      FIXParser* parser, FIXMsg* msg, FIXGroup* group, char const* data, uint32_t len, char delimiter, FIXFieldDescr** fdescr, char const** dbegin, char const** dend);
int fix_parser_validate_attrs(FIXParserAttrs* attrs);
int fix_parser_check_value(FIXFieldDescr* fdescr, char const* dbegin, char const* dend, char delimiter);
int32_t fix_parser_parse_group(FIXParser* parser, FIXMsg* msg, FIXGroup* parentGroup, int64_t groupTag, int64_t numGroups, char const* data, uint32_t len, char delimiter, char const** stop);

#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_PARSER_PRIV_H */
