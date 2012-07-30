/// @file   fix_utils.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/26/2012 04:14:35 PM

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