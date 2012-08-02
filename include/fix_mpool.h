/// @file   fix_mpool.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/31/2012 06:19:35 PM

#ifndef FIX_PARSER_FIX_MPOOL_H
#define FIX_PARSER_FIX_MPOOL_H

#include <stdint.h>

typedef struct FIXMPool_ FIXMPool;

FIXMPool* new_fix_mpool();
void* fix_mpool_alloc(FIXMPool* pool, uint32_t size);
void* fix_mpool_realloc(FIXMPool* pool, void* ptr, uint32_t size);
void free_fix_mpool(FIXMPool* pool);

#endif // FIX_PARSER_FIX_MPOOL_H