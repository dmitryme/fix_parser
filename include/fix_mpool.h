/// @file   fix_mpool.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/31/2012 06:19:35 PM

#ifndef FIX_PARSER_FIX_MPOOL_H
#define FIX_PARSER_FIX_MPOOL_H

#include <stdint.h>

typedef struct Page_
{
   uint32_t size;
   uint32_t offset;
   struct Page_* next;
   char data[1];
} Page;

typedef struct FIXMPool_
{
   uint32_t page_size;
   Page* pages;
   Page* curr_page;
} FIXMPool;

FIXMPool* new_fix_mpool(uint32_t initSize);
void* fix_mpool_alloc(FIXMPool* pool, uint32_t size);
void* fix_mpool_realloc(FIXMPool* pool, void* ptr, uint32_t size);
void free_fix_mpool(FIXMPool* pool);

#endif // FIX_PARSER_FIX_MPOOL_H