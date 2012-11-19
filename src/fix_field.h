/* @file   fix_field.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/25/2012 03:35:40 PM
*/

#ifndef FIX_PARSER_FIX_FIELD_H
#define FIX_PARSER_FIX_FIELD_H

#include "fix_types.h"
#include "fix_msg_priv.h"

#include <stdint.h>
#include <stdlib.h>

#define GROUP_SIZE 64

struct FIXField_
{
   FIXFieldDescr const* descr;
   struct FIXField_* next;
   uint32_t body_len;
   uint32_t size;
   void* data;
} __attribute__((packed));

struct FIXGroup_
{
   FIXField* fields[GROUP_SIZE];
   FIXFieldDescr const* parent_fdescr;
   struct FIXGroup_* next;
} __attribute__((packed));

typedef struct FIXGroups_
{
   FIXGroup* group[1];
} FIXGroups;

FIXField* fix_field_set(FIXMsg* msg, FIXGroup* grp, FIXFieldDescr const* descr, unsigned char const* data, uint32_t len);

FIXField* fix_field_get(FIXMsg* msg, FIXGroup* grp, uint32_t tag);
int32_t fix_field_del(FIXMsg* msg, FIXGroup* grp, uint32_t tag);

FIXGroup* fix_group_add(FIXMsg* msg, FIXGroup* grp, FIXFieldDescr* descr, FIXField** fld);
FIXGroup* fix_group_get(FIXMsg* msg, FIXGroup* tbl, uint32_t tag, uint32_t grpIdx);
int32_t fix_group_del(FIXMsg* msg, FIXGroup* tbl, uint32_t tag, uint32_t grpIdx);

#endif /* FIX_PARSER_FIX_FIELD_H */
