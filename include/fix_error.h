/**
 * @file   fix_error.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 10:54:30 AM
 */

#ifndef FIX_PARSER_FIX_ERROR_H
#define FIX_PARSER_FIX_ERROR_H

#include "fix_types.h"
#include "fix_parser_dll.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * destroy error description
 * @param[in] error - error description to destroy
 */
FIX_PARSER_API void fix_error_free(FIXError* error);

/**
 * return error code during new parser instance creation
 * @param[in] error - pointer to memory with error description
 */
FIX_PARSER_API FIXErrCode fix_error_get_code(FIXError* error);
/**
 * return human readable error description
 * @param[in] error - pointer to memory with error description
 */
FIX_PARSER_API char const* fix_error_get_text(FIXError* error);

#ifdef __cplusplus
}
#endif

#endif // FIX_PARSER_FIX_ERROR_H
