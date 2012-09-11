/* @file   fix_page.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/02/2012 04:34:14 PM
*/

#ifndef FIX_PARSER_FIX_PAGE_H
#define FIX_PARSER_FIX_PAGE_H

#include <stdint.h>

typedef struct FIXPage_
{
   uint32_t size;
   uint32_t offset;
   struct FIXPage_* next;
   char data[1];
} __attribute__((packed)) FIXPage;

#endif /* FIX_PARSER_FIX_PAGE_H */