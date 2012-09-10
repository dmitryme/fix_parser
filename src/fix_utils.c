/* @file   fix_utils.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/02/2012 01:05:17 PM
*/

#include "fix_utils.h"
#include "fix_types.h"

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

int fix_utils_numdigits(long val)
{
   int cnt = 0;
   do
   {
      ++cnt;
      val /= 10;
   }
   while(val);
   return cnt;
}

long lpow10(int n)
{
   static long arr[19] =
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

int fix_utils_ltoa(long val, char* buff, uint32_t buffLen)
{
   if (!buff)
   {
      return FIX_FAILED;
   }
   int i = 0;
   if (val < 0)
   {
      buff[i++] = '-';
      val = -val;
      --buffLen;
   }
   int nd = fix_utils_numdigits(val);
   for(; buffLen && nd; ++i, --nd, --buffLen)
   {
      long pow = lpow10(nd - 1);
      long digit = val/pow;
      buff[i] = digit + 48;
      val -= digit * pow;
   }
   return i;
}

int fix_utils_dtoa(double val, char* buff, uint32_t buffLen)
{
   if (!buff)
   {
      return FIX_FAILED;
   }
   int i = 0;
   if (val < 0)
   {
      buff[i++] = '-';
      val = -val;
      --buffLen;
   }
   long m = (long)val;
   int nd = fix_utils_numdigits(val);
   int j = nd;
   for(; buffLen && j; ++i, --j, --buffLen)
   {
      long pow = lpow10(j - 1);
      int digit = (int)m/pow;
      buff[i] = digit + 48;
      m -= (pow * digit);
   }
   m = (long)(val * lpow10(15 - nd)) - (long)val * lpow10(15 - nd);
   if (m && buffLen)
   {
      buff[i] = '.';
      ++i;
   }
   j = 15 - nd;
   for(; buffLen && m && j; ++i, --j, --buffLen)
   {
      long pow = lpow10(j - 1);
      long digit = m/pow;
      buff[i] = digit + 48;
      m -= (pow * digit);
   }
   return i;
}

int fix_utils_atoi32(char const* buff, uint32_t buffLen, int32_t* val)
{
   if (!val)
   {
      return FIX_FAILED;
   }
   *val = 0;
   int i = 0;
   int32_t sign = 1;
   if (buff[i] == '-')
   {
      sign = -1;
      ++i;
   }
   for(; i < buffLen; ++i)
   {
      if (buff[i] < '0' || buff[i] > '9')
      {
         return FIX_FAILED;
      }
      *val = *val * 10 + (buff[i] - 48);
   }
   *val *= sign;
   return FIX_SUCCESS;
}

int fix_utils_atoi64(char const* buff, uint32_t buffLen, int64_t* val)
{
   if (!val)
   {
      return FIX_FAILED;
   }
   *val = 0;
   int i = 0;
   int64_t sign = 1;
   if (buff[i] == '-')
   {
      sign = -1;
      ++i;
   }
   for(;i < buffLen; ++i)
   {
      if (buff[i] < '0' || buff[i] > '9')
      {
         return FIX_FAILED;
      }
      *val = *val * 10 + (buff[i] - 48);
   }
   *val *= sign;
   return FIX_SUCCESS;
}

int fix_utils_atod(char const* buff, uint32_t buffLen, double* val)
{
   if (!val)
   {
      return FIX_FAILED;
   }
   *val = 0.0;
   int i = 0;
   int32_t sign = 1;
   if (buff[i] == '-')
   {
      sign = -1;
      ++i;
   }
   for(; i < buffLen; ++i)
   {
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
   for(int j = 1;i < buffLen; ++i, ++j)
   {
      if ((buff[i] < '0' || buff[i] > '9'))
      {
         return FIX_FAILED;
      }
      exp += (double)(buff[i] - 48) / lpow10(j);
   }
   *val += exp;
   *val *= sign;
   return FIX_SUCCESS;
}