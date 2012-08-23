/* @file   fix_tag.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/25/2012 03:35:31 PM
*/

#include "fix_tag.h"
#include "fix_parser.h"
#include "fix_parser_priv.h"
#include "fix_message_priv.h"

#include <stdlib.h>
#include <string.h>

typedef struct FIXGroups_
{
   uint32_t cnt;
   FIXGroup* grp[1];
} FIXGroups;

FIXTag* free_fix_tag(FIXMessage* msg, FIXTag* fix_tag);

/*-----------------------------------------------------------------------------------------------------------------------*/
/* PUBLICS                                                                                                               */
/*-----------------------------------------------------------------------------------------------------------------------*/
FIXTag* fix_tag_set(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, unsigned char const* data, uint32_t len)
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
      fix_tag = fix_message_alloc(msg, sizeof(FIXTag));
      if (!fix_tag)
      {
         return NULL;
      }
      fix_tag->type = FIXTagType_Value;
      fix_tag->next = grp->fix_tags[idx];
      fix_tag->num = tagNum;
      grp->fix_tags[idx] = fix_tag;
      fix_tag->data = fix_message_alloc(msg, len);
   }
   else
   {
      fix_tag->data = fix_message_realloc(msg, fix_tag->data, len);
   }
   if (!fix_tag->data)
   {
      return NULL;
   }
   memcpy(fix_tag->data, data, len);
   return fix_tag;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXTag* fix_tag_get(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   FIXTag* it = grp->fix_tags[idx];
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
int fix_tag_del(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   FIXTag* fix_tag = grp->fix_tags[idx];
   FIXTag* prev = fix_tag;
   while(fix_tag)
   {
      if (fix_tag->num == tagNum)
      {
         if (prev == fix_tag)
         {
            grp->fix_tags[idx] = free_fix_tag(msg, fix_tag);
         }
         else
         {
            prev->next = free_fix_tag(msg, fix_tag);
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
FIXGroup* fix_tag_add_group(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum)
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
      fix_tag = fix_message_alloc(msg, sizeof(FIXTag));
      fix_tag->type = FIXTagType_Group;
      fix_tag->num = tagNum;
      fix_tag->next = grp->fix_tags[idx];
      grp->fix_tags[idx] = fix_tag;
      fix_tag->data = fix_message_alloc(msg, sizeof(FIXGroups));
      if (!fix_tag->data)
      {
         return NULL;
      }
      FIXGroups* grps = (FIXGroups*)fix_tag->data;
      grps->cnt = 1;
      grps->grp[0] = fix_message_get_group(msg);
      if (!grps->grp[0])
      {
         return NULL;
      }
   }
   else
   {
      FIXGroups* grps = (FIXGroups*)fix_tag->data;
      FIXGroups* new_grps = fix_message_realloc(msg, fix_tag->data, sizeof(FIXGroups) + sizeof(FIXGroup*) * grps->cnt);
      new_grps->cnt = grps->cnt + 1;
      memcpy(new_grps->grp, grps->grp, sizeof(FIXGroup*) * grps->cnt);
      new_grps->grp[new_grps->cnt - 1] = fix_message_get_group(msg);
      if (!new_grps->grp[new_grps->cnt - 1])
      {
         return NULL;
      }
      fix_tag->data = new_grps;
   }
   FIXGroups* grps = (FIXGroups*)fix_tag->data;
   FIXGroup* new_grp = grps->grp[grps->cnt - 1];
   new_grp->next = msg->used_groups;
   msg->used_groups = new_grp;
   return grp;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_tag_get_group(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx)
{
   int const idx = tagNum % TABLE_SIZE;
   FIXTag* it = grp->fix_tags[idx];
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
         return grps->grp[grpIdx];
      }
      else
      {
         it = it->next;
      }
   }
   return NULL;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int fix_tag_del_group(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx)
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
   fix_message_free_group(msg, grps->grp[grpIdx]);
   grps->cnt -= 1;
   if (grps->cnt == grpIdx)
   {
      grps->grp[grps->cnt] = NULL;
   }
   else
   {
      memcpy(&grps->grp[grpIdx], &grps->grp[grpIdx + 1], grps->cnt - grpIdx);
      grps->grp[grps->cnt] = NULL;
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
FIXTag* free_fix_tag(FIXMessage* msg, FIXTag* fix_tag)
{
   if (fix_tag->type == FIXTagType_Group)
   {
      FIXGroups* grps = (FIXGroups*)fix_tag->data;
      for(int i = 0; i < grps->cnt; ++i)
      {
         fix_parser_free_group(msg->parser, grps->grp[i]);
      }
   }
   return fix_tag->next;
}