/// @file   fix_tag.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/25/2012 03:35:40 PM

#ifndef FIX_PARSER_FIX_TAG_H
#define FIX_PARSER_FIX_TAG_H

#include "fix_types.h"

#include <stdint.h>

#define TABLE_SIZE 64

struct FIXTag_
{
   uint32_t num;
   FIXTagType type;
   uint32_t size;
   struct FIXTag_* next;
   union
   {
      FIXTagTable** grpTbl;
      unsigned char data[1];
   };
} __attribute__((packed));

FIXTagTable* new_fix_table();
void free_fix_table(FIXTagTable* tbl);

FIXTag* set_fix_table_tag(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum, unsigned char const* data, uint32_t len);
FIXTag* get_fix_table_tag(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum);
int del_fix_table_tag(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum);

FIXTagTable* add_fix_table_group(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum);
FIXTagTable* get_fix_table_group(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum, uint32_t grpIdx);
int del_fix_table_group(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum, uint32_t grpIdx);

#endif // FIX_PARSER_FIX_TAG_H