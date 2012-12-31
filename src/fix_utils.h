/* @file   fix_utils.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/26/2012 04:14:35 PM
*/

#ifndef FIX_PARSER_FIX_UTILS_H
#define FIX_PARSER_FIX_UTILS_H

#include <fix_types.h>

#ifndef WIN32
#  define LIKE(x)    __builtin_expect(!!(x), 1)
#  define UNLIKE(x)  __builtin_expect(!!(x), 0)
#  define _strdup strdup
#else
#  define LIKE(x) x
#  define UNLIKE(x) x
#  define PATH_MAX 4096
#endif

#ifdef __cplusplus
extern "C"
{
#endif

uint32_t fix_utils_hash_string(char const* s);
int32_t fix_utils_numdigits(int64_t val);
int64_t fix_utils_lpow10(int32_t n);
int32_t fix_utils_i64toa(int64_t val, char* buff, uint32_t buffLen, char padSym);
int32_t fix_utils_dtoa(double val, char* buff, uint32_t buffLen);
int32_t fix_utils_atoi32(char const* buff, uint32_t buffLen, char stopChar, int32_t* val);
int32_t fix_utils_atoi64(char const* buff, uint32_t buffLen, char stopChar, int64_t* val);
int32_t fix_utils_atod(char const* buff, uint32_t buffLen, char stopChar, double* val);

FIXErrCode fix_utils_make_path(char const* protocolFile, char const* transpFile, char* path, uint32_t buffLen);


#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_UTILS_H */
