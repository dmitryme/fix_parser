/* @file   fix_utils.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/02/2012 01:05:17 PM
*/

#include "fix_utils.h"

uint32_t hash_string(char const* s)
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

int numdigits(long val)
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

int ltoa(long val, char* buff, uint32_t buffLen)
{
   int nd = numdigits(val);
   int i = 0;
   for(; nd; ++i, --nd)
   {
      long pow = lpow10(nd - 1);
      long digit = val/pow;
      buff[i] = digit + 48;
      val -= digit * pow;
   }
   return i;
}

int dtoa(double val, char* buff, uint32_t buffLen)
{
   long m = (long)val;
   int nd = numdigits(val);
   int j = nd;
   int i = 0;
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