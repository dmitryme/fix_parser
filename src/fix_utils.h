/* @file   fix_utils.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/26/2012 04:14:35 PM
*/

#ifndef FIX_PARSER_FIX_UTILS_H
#define FIX_PARSER_FIX_UTILS_H

#include <stdint.h>

#define LIKE(x)    __builtin_expect(!!(x), 1)
#define UNLIKE(x)  __builtin_expect(!!(x), 0)

uint32_t hash_string(char const* s);
int numdigits(long val);
long lpow10(int n);
int ltoa(long val, char* buff, uint32_t buffLen);
int dtoa(double val, char* buff, uint32_t buffLen);

#endif /* FIX_PARSER_FIX_UTILS_H */
