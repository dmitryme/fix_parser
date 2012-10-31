/* @file   fix_error.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:54:30 AM
*/

#ifndef FIX_PARSER_FIX_ERROR_H
#define FIX_PARSER_FIX_ERROR_H

#include "fix_types.h"

#include <stdint.h>

int32_t get_fix_parser_error_code(FIXParser* parser);
char const* get_fix_parser_error_text(FIXParser* parser);

int32_t get_fix_error_code();
char const* get_fix_error_text();

#endif // FIX_PARSER_FIX_ERROR_H
