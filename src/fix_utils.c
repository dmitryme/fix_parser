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
