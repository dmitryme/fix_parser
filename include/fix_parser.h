/* @file   fix_parser.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:54:30 AM
*/

#ifndef FIX_PARSER_FIX_PARSER_H
#define FIX_PARSER_FIX_PARSER_H

#include "fix_types.h"
#include "fix_msg.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

FIXParser* fix_parser_create(char const* protFile, FIXParserAttrs const* attrs, int32_t flags);
void fix_parser_free(FIXParser* parser);
FIXMsg* fix_parser_fix_to_msg(FIXParser* parser, char const* data, uint32_t len, char delimiter, char const** stop);
int32_t fix_parser_get_error_code(FIXParser* parser);
char const* fix_parser_get_error_text(FIXParser* parser);

#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_PARSER_H */
