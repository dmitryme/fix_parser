/* @file   fix_tag.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/25/2012 03:35:31 PM
*/

#include "fix_tag.h"
#include "fix_parser.h"
#include "fix_parser_priv.h"
#include "fix_msg_priv.h"

#include <stdlib.h>
#include <string.h>

FIXTag* fix_tag_free(FIXMsg* msg, FIXTag* fix_tag);
void fix_group_free(FIXMsg* msg, FIXGroup* group);

/*-----------------------------------------------------------------------------------------------------------------------*/
/* PUBLICS                                                                                                               */
/*-----------------------------------------------------------------------------------------------------------------------*/
FIXTag* fix_tag_set(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, unsigned char const* data, uint16_t len)
{
   FIXTag* fix_tag = fix_tag_get(msg, grp, tagNum);
   if (!fix_tag && get_fix_error_code(msg->parser))
   {
      return NULL;
   }
   if (fix_tag && fix_tag->type == FIXTagType_Group)
   {
      fix_parser_set_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
      return NULL;
   }
   int idx = tagNum % TABLE_SIZE;
   if (!fix_tag)
   {
      fix_tag = fix_msg_alloc(msg, sizeof(FIXTag));
      if (!fix_tag)
      {
         return NULL;
      }
      fix_tag->type = FIXTagType_Value;
      fix_tag->next = grp->tags[idx];
      fix_tag->num = tagNum;
      grp->tags[idx] = fix_tag;
      fix_tag->data = fix_msg_alloc(msg, sizeof(uint16_t) + len);
   }
   else
   {
      fix_tag->data = fix_msg_realloc(msg, fix_tag->data, sizeof(uint16_t) + len);
   }
   if (!fix_tag->data)
   {
      return NULL;
   }
   *(uint16_t*)fix_tag->data = len;
   memcpy(fix_tag->data + sizeof(uint16_t), data, len);
   return fix_tag;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXTag* fix_tag_get(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   FIXTag* it = grp->tags[idx];
   while(it)
   {
      if (it->num == tagNum)
      {
         return it;
      }
      else
      {
         it = it->next;
      }
   }
   return NULL;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int fix_tag_del(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   FIXTag* fix_tag = grp->tags[idx];
   FIXTag* prev = fix_tag;
   while(fix_tag)
   {
      if (fix_tag->num == tagNum)
      {
         if (prev == fix_tag)
         {
            grp->tags[idx] = fix_tag_free(msg, fix_tag);
         }
         else
         {
            prev->next = fix_tag_free(msg, fix_tag);
         }
         return FIX_SUCCESS;
      }
      prev = fix_tag;
      fix_tag = fix_tag->next;
   }
   fix_parser_set_error(msg->parser, FIX_ERROR_TAG_NOT_FOUND, "FIXTag not found");
   return FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_group_add(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum)
{
   FIXTag* fix_tag = fix_tag_get(msg, grp, tagNum);
   if (fix_tag && fix_tag->type != FIXTagType_Group)
   {
      fix_parser_set_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
      return NULL;
   }
   if (!fix_tag)
   {
      uint32_t const idx = tagNum % TABLE_SIZE;
      fix_tag = fix_msg_alloc(msg, sizeof(FIXTag));
      fix_tag->type = FIXTagType_Group;
      fix_tag->num = tagNum;
      fix_tag->next = grp->tags[idx];
      grp->tags[idx] = fix_tag;
      fix_tag->data = fix_msg_alloc(msg, sizeof(FIXGroups));
      if (!fix_tag->data)
      {
         return NULL;
      }
      FIXGroups* grps = (FIXGroups*)fix_tag->data;
      grps->cnt = 1;
      grps->group[0] = fix_msg_alloc_group(msg);
      if (!grps->group[0])
      {
         return NULL;
      }
   }
   else
   {
      FIXGroups* grps = (FIXGroups*)fix_tag->data;
      FIXGroups* new_grps = fix_msg_realloc(msg, fix_tag->data, sizeof(FIXGroups) + sizeof(FIXGroup*) * grps->cnt);
      new_grps->cnt = grps->cnt + 1;
      memcpy(new_grps->group, grps->group, sizeof(FIXGroup*) * grps->cnt);
      new_grps->group[new_grps->cnt - 1] = fix_msg_alloc_group(msg);
      if (!new_grps->group[new_grps->cnt - 1])
      {
         return NULL;
      }
      fix_tag->data = new_grps;
   }
   FIXGroups* grps = (FIXGroups*)fix_tag->data;
   FIXGroup* new_grp = grps->group[grps->cnt - 1];
   return new_grp;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_group_get(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx)
{
   int const idx = tagNum % TABLE_SIZE;
   FIXTag* it = grp->tags[idx];
   while(it)
   {
      if (it->num == tagNum)
      {
         if (it->type != FIXTagType_Group)
         {
            fix_parser_set_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
            return NULL;
         }
         FIXGroups* grps = (FIXGroups*)it->data;
         if (grpIdx >= grps->cnt)
         {
            fix_parser_set_error(msg->parser, FIX_ERROR_GROUP_WRONG_INDEX, "Wrong index");
            return NULL;
         }
         return grps->group[grpIdx];
      }
      else
      {
         it = it->next;
      }
   }
   return NULL;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int fix_group_del(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx)
{
   FIXTag* fix_tag = fix_tag_get(msg, grp, tagNum);
   if (!fix_tag)
   {
      return FIX_FAILED;
   }
   if (fix_tag->type != FIXTagType_Group)
   {
      fix_parser_set_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
      return FIX_FAILED;
   }
   FIXGroups* grps = (FIXGroups*)fix_tag->data;
   fix_group_free(msg, grps->group[grpIdx]);
   grps->cnt -= 1;
   if (grps->cnt == grpIdx)
   {
      grps->group[grps->cnt] = NULL;
   }
   else
   {
      memcpy((char*)grps->group + sizeof(FIXGroup*) * grpIdx, (char*)grps->group + sizeof(FIXGroup*) * (grpIdx + 1),
         sizeof(FIXGroup*) * (grps->cnt - grpIdx));
      grps->group[grps->cnt] = NULL;
   }
   if (!grps->cnt) // all groups has been deleted, so tag will be deleted either
   {
      return fix_tag_del(msg, grp, tagNum);
   }
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
/* PRIVATES                                                                                                               */
/*------------------------------------------------------------------------------------------------------------------------*/
FIXTag* fix_tag_free(FIXMsg* msg, FIXTag* fix_tag)
{
   if (fix_tag->type == FIXTagType_Group)
   {
      FIXGroups* grps = (FIXGroups*)fix_tag->data;
      for(int i = 0; i < grps->cnt; ++i)
      {
         fix_group_free(msg, grps->group[i]);
      }
   }
   return fix_tag->next;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_group_free(FIXMsg* msg, FIXGroup* group)
{
   for(int i = 0; i < TABLE_SIZE; ++i)
   {
      FIXTag* tag = group->tags[i];
      while(tag)
      {
         tag = fix_tag_free(msg, tag);
      }
   }
   fix_msg_free_group(msg, group);
}