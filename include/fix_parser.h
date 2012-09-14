/* @file   fix_parser.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:54:30 AM
*/

#ifndef FIX_PARSER_FIX_PARSER_H
#define FIX_PARSER_FIX_PARSER_H

#include "fix_types.h"
#include "fix_msg.h"

#include <stdint.h>

FIXParser* fix_parser_create(
      uint32_t pageSize, uint32_t maxPageSize, uint32_t numPages, uint32_t maxPages,
      uint32_t numGroups, uint32_t maxGroups, int32_t flags);
void fix_parser_free(FIXParser* parser);

int32_t get_fix_error_code(FIXParser* parser);
char const* get_fix_error_text(FIXParser* parser);
int32_t get_fix_parser_flags(FIXParser* parser);

int32_t fix_protocol_init(FIXParser*, char const* protFile);
int32_t parse_fix(FIXParser* parser, char const* data, uint32_t len, FIXProtocolVerEnum ver, char delimiter, FIXMsg** msg);

#endif /* FIX_PARSER_FIX_PARSER_H */