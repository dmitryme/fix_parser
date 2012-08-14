/* @file   fix_parser_priv.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:54:30 AM
*/

#ifndef FIX_PARSER_FIX_PARSER_PRIV_H
#define FIX_PARSER_FIX_PARSER_PRIV_H

#include "fix_types.h"
#include "fix_protocol_descr.h"
#include "fix_page.h"
#include "fix_tag.h"

#include <stdint.h>
#include <stdarg.h>

#define SOH 0x01
#define ERROR_TXT_SIZE 1024

struct FIXParser_
{
   FIXProtocolDescr* protocols[FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT - 1];
   int err_code;
   char err_text[ERROR_TXT_SIZE];
   uint32_t flags;
   FIXPage* pages;
   FIXPage* free_page;
   uint32_t num_pages;
   uint32_t max_pages;
   uint32_t page_size;
   FIXGroup* groups;
   FIXGroup* free_group;
   uint32_t num_groups;
   uint32_t max_groups;
};

FIXPage* fix_parser_get_page(FIXParser* parser, uint32_t pageSize);
void fix_parser_free_page(FIXParser* parser, FIXPage* page);
FIXGroup* fix_parser_get_group(FIXParser* parser);
void fix_parser_free_group(FIXParser* parser, FIXGroup* group);
void set_fix_va_error(FIXParser* parser, int code, char const* text, va_list ap);
void set_fix_error(FIXParser* parser, int code, char const* text, ...);
void reset_fix_error(FIXParser* parser);
FIXProtocolDescr* get_fix_protocol_descr(FIXParser* parser, FIXProtocolVerEnum ver);

#endif /* FIX_PARSER_FIX_PARSER_PRIV_H */
