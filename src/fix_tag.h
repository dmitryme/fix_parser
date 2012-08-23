/* @file   fix_tag.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/25/2012 03:35:40 PM
*/

#ifndef FIX_PARSER_FIX_TAG_H
#define FIX_PARSER_FIX_TAG_H

#include "fix_types.h"

#include <stdint.h>

#define TABLE_SIZE 64

struct FIXTag_
{
   uint32_t num;
   FIXTagType type;
   struct FIXTag_* next;
   void* data;
} __attribute__((packed));

typedef struct FIXTagTable_
{
   FIXTag* fix_tags[TABLE_SIZE];
   struct FIXTagTable_* next;
} __attribute__((packed)) FIXGroup;

FIXTag* fix_tag_set(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, unsigned char const* data, uint32_t len);
FIXTag* fix_tag_get(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum);
int fix_tag_del(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum);

FIXGroup* fix_tag_add_group(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum);
FIXGroup* fix_tag_get_group(FIXMessage* msg, FIXGroup* tbl, uint32_t tagNum, uint32_t grpIdx);
int fix_tag_del_group(FIXMessage* msg, FIXGroup* tbl, uint32_t tagNum, uint32_t grpIdx);

#endif /* FIX_PARSER_FIX_TAG_H */