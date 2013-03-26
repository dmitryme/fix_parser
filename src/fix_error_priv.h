/**
 * @file   fix_error_priv.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 10:54:30 AM
 */

#ifndef FIX_PARSER_FIX_ERROR_PRIV_H
#define FIX_PARSER_FIX_ERROR_PRIV_H

#include <fix_types.h>

#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ERROR_TXT_SIZE 96 ///< maximum error description size

/**
 * hold error description
 */
struct FIXError_
{
   FIXErrCode code;  ///< error code
   char       text[ERROR_TXT_SIZE]; ///< error description
};

/**
 * create error using variable arguments
 * @param[in] code - new error code
 * @param[in] text - error description pattern
 * @param[in] ap - arguments
 * @return error description
 */
FIXError* fix_error_create_va(FIXErrCode code, char const* text, va_list ap);

/**
 * create error using variable arguments
 * @param[in] code - new error code
 * @param[in] text - error description pattern
 * @return error description
 */
FIXError* fix_error_create(FIXErrCode code, char const* text, ...);

#ifdef __cplusplus
}
#endif

#endif // FIX_PARSER_FIX_ERROR_PRIV_H
