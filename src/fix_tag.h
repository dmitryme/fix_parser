/* @file   fix_tag.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/25/2012 03:35:40 PM
*/

#ifndef FIX_PARSER_FIX_TAG_H
#define FIX_PARSER_FIX_TAG_H

#include "fix_types.h"

#include <stdint.h>

#define GROUP_SIZE 64

struct FIXTag_
{
   uint32_t num;
   FIXTagTypeEnum type;
   struct FIXTag_* next;
   uint32_t body_len;
   uint32_t size;
   void* data;
} __attribute__((packed));

typedef struct FIXTagTable_
{
   FIXTag* tags[GROUP_SIZE];
   struct FIXGroup_* next;
} __attribute__((packed)) FIXGroup;

typedef struct FIXGroups_
{
   FIXGroup* group[1];
} FIXGroups;

FIXTag* fix_tag_set(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, unsigned char const* data, uint16_t len);
FIXTag* fix_tag_get(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);
int fix_tag_del(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);

FIXGroup* fix_group_add(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, FIXTag** tag);
FIXGroup* fix_group_get(FIXMsg* msg, FIXGroup* tbl, uint32_t tagNum, uint32_t grpIdx);
int fix_group_del(FIXMsg* msg, FIXGroup* tbl, uint32_t tagNum, uint32_t grpIdx);

#endif /* FIX_PARSER_FIX_TAG_H */