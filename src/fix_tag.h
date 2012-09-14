/* @file   fix_tag.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/25/2012 03:35:40 PM
*/

#ifndef FIX_PARSER_FIX_TAG_H
#define FIX_PARSER_FIX_TAG_H

#include "fix_types.h"
#include "fix_msg_priv.h"

#include <stdint.h>
#include <stdlib.h>

#define GROUP_SIZE 64

#define FIND_TAG_STEP \
if (!it) return NULL; \
if (it->num == tagNum) return it; \
it = it->next;

struct FIXTag_
{
   uint32_t num;
   FIXTagTypeEnum type;
   struct FIXTag_* next;
   uint32_t body_len;
   uint32_t size;
   void* data;
} __attribute__((packed));

struct FIXGroup_
{
   FIXTag* tags[GROUP_SIZE];
   FIXFieldDescr* parent_fdescr;
   struct FIXGroup_* next;
} __attribute__((packed));

typedef struct FIXGroups_
{
   FIXGroup* group[1];
} FIXGroups;

FIXTag* fix_tag_set(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, unsigned char const* data, uint32_t len);

inline static FIXTag* fix_tag_get(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum)
{
   uint32_t const idx = tagNum % GROUP_SIZE;
   FIXTag* it = (grp ? grp : msg->tags)->tags[idx];
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;FIND_TAG_STEP;
   return NULL;
}

int32_t fix_tag_del(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum);

FIXGroup* fix_group_add(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, FIXTag** tag);
FIXGroup* fix_group_get(FIXMsg* msg, FIXGroup* tbl, uint32_t tagNum, uint32_t grpIdx);
int32_t fix_group_del(FIXMsg* msg, FIXGroup* tbl, uint32_t tagNum, uint32_t grpIdx);

#endif /* FIX_PARSER_FIX_TAG_H */