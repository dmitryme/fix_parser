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

#define ERROR_TXT_SIZE 1024 ///< maximum error description size

/**
 * hold error description
 */
typedef struct FIXError
{
   FIXErrCode code;  ///< error code
   char       text[ERROR_TXT_SIZE]; ///< error description
} FIXError;

/**
 * set error using variable arguments
 * @param[in] error - error, which is being set
 * @param[in] code - new error code
 * @param[in] text - error description pattern
 * @param[in] ap - arguments
 */
void fix_error_set_va(FIXError* error, FIXErrCode code, char const* text, va_list ap);

/**
 * set error using variable arguments
 * @param[in] error - error, which is being set
 * @param[in] code - new error code
 * @param[in] text - error description pattern
 */
void fix_error_set(FIXError* error, FIXErrCode code, char const* text, ...);

/**
 * reset error
 * @param[in] error - error, which is begin reset
 */
void fix_error_reset(FIXError* error);

/**
 * return global, not bound to any parser instance, error
 * @return error pointer
 */
FIXError* fix_error_static_get();

/**
 * set global error
 */
void fix_error_static_set_va(FIXErrCode code, char const* text, va_list ap);

/**
 * set global error
 */
void fix_error_static_set(FIXErrCode code, char const* text, ...);

/**
 * reset global error
 */
void fix_error_static_reset();

#ifdef __cplusplus
}
#endif

#endif // FIX_PARSER_FIX_ERROR_PRIV_H
