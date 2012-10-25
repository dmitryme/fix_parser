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

#include <stdint.h>
#include <stdarg.h>

#define SOH 0x01
#define ERROR_TXT_SIZE 1024

struct FIXParser_
{
   FIXProtocolDescr* protocol;
   int32_t err_code;
   char err_text[ERROR_TXT_SIZE];
   FIXParserAttrs attrs;
   int32_t  flags;
   FIXPage* page;
   uint32_t used_pages;
   FIXGroup* group;
   uint32_t used_groups;
};

int validate_attrs(FIXParserAttrs* attrs);
FIXPage* fix_parser_alloc_page(FIXParser* parser, uint32_t pageSize);
FIXPage* fix_parser_free_page(FIXParser* parser, FIXPage* page);
FIXGroup* fix_parser_alloc_group(FIXParser* parser);
FIXGroup* fix_parser_free_group(FIXParser* parser, FIXGroup* group);
void fix_parser_set_va_error(FIXParser* parser, int32_t code, char const* text, va_list ap);
void fix_parser_set_error(FIXParser* parser, int32_t code, char const* text, ...);
void fix_parser_reset_error(FIXParser* parser);
int64_t parse_field(
      FIXParser* parser, char const* data, uint32_t len, char delimiter, char const** dbegin, char const** dend);

#endif /* FIX_PARSER_FIX_PARSER_PRIV_H */