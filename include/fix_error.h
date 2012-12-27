/* @file   fix_error.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:54:30 AM
*/

#ifndef FIX_PARSER_FIX_ERROR_H
#define FIX_PARSER_FIX_ERROR_H

#include "fix_types.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

FIXErrCode fix_error_get_code();
char const* fix_error_get_text();

#ifdef __cplusplus
}
#endif

#endif // FIX_PARSER_FIX_ERROR_H
