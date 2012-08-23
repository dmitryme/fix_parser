/* @file   fix_parser.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:54:30 AM
*/

#ifndef FIX_PARSER_FIX_PARSER_H
#define FIX_PARSER_FIX_PARSER_H

#include "fix_types.h"
#include "fix_message.h"

#include <stdint.h>

FIXParser* new_fix_parser(
      uint32_t pageSize, uint32_t maxPageSize, uint32_t numPages, uint32_t maxPages,
      uint32_t numGroups, uint32_t maxGroups, FIXParserFlags flags);
void free_fix_parser(FIXParser* parser);

int get_fix_error_code(FIXParser* parser);
char const* get_fix_error_text(FIXParser* parser);
int get_fix_parser_flags(FIXParser* parser);

int fix_protocol_init(FIXParser*, char const* protFile);
int parse_fix(FIXParser* parser, FIXMessage** msg, char const* data, uint32_t len);

#endif /* FIX_PARSER_FIX_PARSER_H */