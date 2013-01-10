/**
 * @file   fix_page.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 08/02/2012 04:34:14 PM
 */

#ifndef FIX_PARSER_FIX_PAGE_H
#define FIX_PARSER_FIX_PAGE_H

#include <stdint.h>

#pragma pack(push, 1)
#pragma pack(1)

/**
 * page of memory for storing FIX fields
 */
typedef struct FIXPage_
{
   uint32_t size;          ///< size of page
   uint32_t offset;        ///< offset of free space
   struct FIXPage_* next;  ///< next page
   char data[1];           ///< start of data
} FIXPage;

#pragma pack(pop)

#endif /* FIX_PARSER_FIX_PAGE_H */
