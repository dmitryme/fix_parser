/**
 * @file   fix_utils.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 08/02/2012 01:05:17 PM
 */

#include "fix_utils.h"
#include "fix_types.h"

#include <stdlib.h>

#define DOUBLE_MAX_DIGITS 15

/*-----------------------------------------------------------------------------------------------------------------------*/
uint32_t fix_utils_hash_string(char const* s)
{
    uint32_t hash = 0;

    for(; *s; ++s)
    {
        hash += *s;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
int32_t fix_utils_numdigits(int64_t val)
{
   int32_t cnt = 0;
   do
   {
      ++cnt;
      val /= 10;
   }
   while(val);
   return cnt;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
int64_t fix_utils_lpow10(int32_t n)
{
   static int64_t arr[19] =
   {
      1,
      10,
      100,
      1000,
      10000,
      100000,
      1000000,
      10000000,
      100000000,
      1000000000,
      10000000000,
      100000000000,
      1000000000000,
      10000000000000,
      100000000000000,
      1000000000000000,
      10000000000000000,
      100000000000000000,
      1000000000000000000
   };
   return arr[n];
}

/*-----------------------------------------------------------------------------------------------------------------------*/
int32_t fix_utils_i64toa(int64_t val, char* buff, uint32_t buffLen, char padSym)
{
   int32_t i = 0;
   if (val < 0)
   {
      buff[i++] = '-';
      val = -val;
      --buffLen;
   }
   int32_t nd = fix_utils_numdigits(val);
   for(uint32_t len = buffLen - nd; len > 0 && padSym; --len)
   {
      buff[i++] = padSym;
   }
   for(; buffLen && nd; ++i, --nd, --buffLen)
   {
      int64_t pow = fix_utils_lpow10(nd - 1);
      int64_t digit = val/pow;
      buff[i] = digit + 48;
      val -= (pow * digit);
   }
   return i + nd;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
int32_t fix_utils_dtoa(double val, char* buff, uint32_t buffLen)
{
   int32_t i = 0;
   if (val < 0)
   {
      buff[i++] = '-';
      val = -val;
      --buffLen;
   }
   int64_t m = (int64_t)val;
   int32_t nd = fix_utils_numdigits(val);
   int32_t j = nd;
   for(; buffLen && j; ++i, --j, --buffLen)
   {
      int64_t pow = fix_utils_lpow10(j - 1);
      int32_t digit = (int32_t)m/pow;
      buff[i] = digit + 48;
      m -= (pow * digit);
   }
   i += j;
   m = (int64_t)(val * fix_utils_lpow10(DOUBLE_MAX_DIGITS - nd)) - (int64_t)val * fix_utils_lpow10(DOUBLE_MAX_DIGITS - nd);
   if (m)
   {
      if (buffLen)
      {
         buff[i] = '.';
      }
      ++i;
   }
   j = DOUBLE_MAX_DIGITS - nd;
   for(; buffLen && m && j; ++i, --j, --buffLen)
   {
      int64_t pow = fix_utils_lpow10(j - 1);
      int64_t digit = m/pow;
      buff[i] = digit + 48;
      m -= (pow * digit);
   }
   for(; m && j; ++i, --j)
   {
      int64_t pow = fix_utils_lpow10(j - 1);
      int64_t digit = m/pow;
      m -= (pow * digit);
   }
   return i;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
int32_t fix_utils_atoi32(char const* buff, uint32_t buffLen, char stopChar, int32_t* val)
{
   *val = 0;
   uint32_t i = 0;
   int32_t sign = 1;
   if (buff[i] == '-')
   {
      sign = -1;
      ++i;
   }
   for(;i < buffLen; ++i)
   {
      if (stopChar && stopChar == buff[i])
      {
         break;
      }
      if (buff[i] < '0' || buff[i] > '9')
      {
         return FIX_FAILED;
      }
      *val = *val * 10 + (buff[i] - 48);
   }
   *val *= sign;
   return i;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
int32_t fix_utils_atoi64(char const* buff, uint32_t buffLen, char stopChar, int64_t* val)
{
   *val = 0;
   uint32_t i = 0;
   int64_t sign = 1;
   if (buff[i] == '-')
   {
      sign = -1;
      ++i;
   }
   for(;i < buffLen; ++i)
   {
      if (stopChar && stopChar == buff[i])
      {
         break;
      }
      if (buff[i] < '0' || buff[i] > '9')
      {
         return FIX_FAILED;
      }
      *val = *val * 10 + (buff[i] - 48);
   }
   *val *= sign;
   return i;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
int32_t fix_utils_atod(char const* buff, uint32_t buffLen, char stopChar, double* val)
{
   *val = 0.0;
   uint32_t i = 0;
   int32_t sign = 1;
   if (buff[i] == '-')
   {
      sign = -1;
      ++i;
   }
   for(; i < buffLen; ++i)
   {
      if (stopChar && stopChar == buff[i])
      {
         break;
      }
      if (buff[i] == '.')
      {
         ++i;
         break;
      }
      if ((buff[i] < '0' || buff[i] > '9'))
      {
         return FIX_FAILED;
      }
      *val = *val * 10 + (buff[i] - 48);
   }
   double exp = 0.0;
   for(int32_t j = 1;i < buffLen; ++i, ++j)
   {
      if (stopChar && stopChar == buff[i])
      {
         break;
      }
      if ((buff[i] < '0' || buff[i] > '9'))
      {
         return FIX_FAILED;
      }
      exp += (double)(buff[i] - 48) / fix_utils_lpow10(j);
   }
   *val += exp;
   *val *= sign;
   return i;
}
