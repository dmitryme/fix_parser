/**
 * @file   fix_utils.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/26/2012 04:14:35 PM
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

/**
 * calculate string hash value
 * @param[in] s - string for hash calculation
 * @return hash value
 */
uint32_t fix_utils_hash_string(char const* s);

/**
 * return number of digits. E.g. 10221 -> 5
 * @param[in] val - numeric value
 * @return number of digits
 */
int32_t fix_utils_numdigits(int64_t val);

/**
 * pow10 calculation
 */
int64_t fix_utils_lpow10(int32_t n);

/**
 * convert int64 value into string
 * @param[in] val     - converted value
 * @param[out] buff   - buffer with converted value
 * @param[in] buffLen - length of buffer
 * @param[in] padSym  - padding symbol
 * @return how many characters written (can be written). If this value greater than buffLen, value converted
 * incompletely
 */
int32_t fix_utils_i64toa(int64_t val, char* buff, uint32_t buffLen, char padSym);

/**
 * convert double to string
 * @param[in] val - converted value
 * @param[out] buff - buffer with converted value
 * @param[in] buffLen - length of buffer
 * @return how many characters written (can be written). If this value greater than buffLen, value converted
 * incompletely
 */
int32_t fix_utils_dtoa(double val, char* buff, uint32_t buffLen);

/**
 * convert string to 32-bit number
 * @param[in] buff - string value
 * @param[in] buffLen - length of buffer
 * @param[in] stopChar - stop parsing on this char. If stopChar == 0, processed till buffer end
 * @param[out] val - converted value
 * @return how many characters processed
 */
int32_t fix_utils_atoi32(char const* buff, uint32_t buffLen, char stopChar, int32_t* val);

/**
 * convert string to 64-bit number
 * @param[in] buff - string value
 * @param[in] buffLen - length of buffer
 * @param[in] stopChar - stop parsing on this char. If stopChar == 0, processed till buffer end
 * @param[out] val - converted value
 * @return how many characters processed
 */
int32_t fix_utils_atoi64(char const* buff, uint32_t buffLen, char stopChar, int64_t* val);

/**
 * convert string to double
 * @param[in] buff - string value
 * @param[in] buffLen - length of buffer
 * @param[in] stopChar - stop parsing on this char. If stopChar == 0, processed till buffer end
 * @param[out] val - converted value
 * @return how many characters processed
 */
int32_t fix_utils_atod(char const* buff, uint32_t buffLen, char stopChar, double* val);

/**
 * fix transpFile path according to protocolFile path
 * @param[in] protocolFile - path to protocol file
 * @param[in] transpFile - path to transport file
 * @param[out] path - fixed transport file path
 * @param[in] buffLen - legth of path buffer
 * @return FIX_SUCCESS - ok, FIX_FAILED - error (buff of too small)
 * @note if transport file path is absolute no fix are made, else path is given from protocol file
 */
FIXErrCode fix_utils_make_path(char const* protocolFile, char const* transpFile, char* path, uint32_t buffLen);


#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_UTILS_H */
