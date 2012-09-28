/* @file   fix_tag.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/25/2012 03:35:31 PM
*/

#include "fix_tag.h"
#include "fix_parser.h"
#include "fix_parser_priv.h"
#include "fix_msg_priv.h"
#include "fix_utils.h"

#include <stdlib.h>
#include <string.h>

FIXTag* fix_tag_free(FIXMsg* msg, FIXTag* fix_tag);
void fix_group_free(FIXMsg* msg, FIXGroup* group);

/*-----------------------------------------------------------------------------------------------------------------------*/
/* PUBLICS                                                                                                               */
/*-----------------------------------------------------------------------------------------------------------------------*/
FIXTag* fix_tag_set(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, unsigned char const* data, uint32_t len)
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
   int32_t idx = tagNum % GROUP_SIZE;
   if (!fix_tag)
   {
      fix_tag = fix_msg_alloc(msg, sizeof(FIXTag));
      if (!fix_tag)
      {
         return NULL;
      }
      fix_tag->type = FIXTagType_Value;
      FIXGroup* group = (grp ? grp : msg->tags);
      fix_tag->next = group->tags[idx];
      fix_tag->num = tagNum;
      group->tags[idx] = fix_tag;
      fix_tag->size = len;
      fix_tag->data = fix_msg_alloc(msg, len);
      fix_tag->body_len = 0;
   }
   else
   {
      fix_tag->size = len;
      fix_tag->data = fix_msg_realloc(msg, fix_tag->data, len);
      msg->body_len -= fix_tag->body_len;
   }
   if (!fix_tag->data)
   {
      return NULL;
   }
   if (LIKE(fix_tag->num != FIXTagNum_BeginString && fix_tag->num != FIXTagNum_BodyLength && fix_tag->num != FIXTagNum_CheckSum))
   {
      fix_tag->body_len = fix_utils_numdigits(tagNum) + 1 + len + 1;
   }
   memcpy(fix_tag->data, data, len);
   msg->body_len += fix_tag->body_len;
   return fix_tag;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_tag_del(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum)
{
   uint32_t const idx = tagNum % GROUP_SIZE;
   FIXGroup* group = grp ? grp : msg->tags;
   FIXTag* fix_tag = group->tags[idx];
   FIXTag* prev = fix_tag;
   while(fix_tag)
   {
      if (fix_tag->num == tagNum)
      {
         if (prev == fix_tag)
         {
            group->tags[idx] = fix_tag_free(msg, fix_tag);
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
FIXGroup* fix_group_add(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, FIXTag** tag)
{
   FIXTag* fix_tag = fix_tag_get(msg, grp, tagNum);
   FIXGroup* group = grp ? grp : msg->tags;
   if (fix_tag && fix_tag->type != FIXTagType_Group)
   {
      fix_parser_set_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
      return NULL;
   }
   if (!fix_tag)
   {
      uint32_t const idx = tagNum % GROUP_SIZE;
      fix_tag = fix_msg_alloc(msg, sizeof(FIXTag));
      fix_tag->type = FIXTagType_Group;
      fix_tag->num = tagNum;
      fix_tag->next = group->tags[idx];
      group->tags[idx] = fix_tag;
      fix_tag->data = fix_msg_alloc(msg, sizeof(FIXGroups));
      if (!fix_tag->data)
      {
         return NULL;
      }
      FIXGroups* grps = (FIXGroups*)fix_tag->data;
      fix_tag->size = 1;
      fix_tag->body_len = 0;
      grps->group[0] = fix_msg_alloc_group(msg);
      if (!grps->group[0])
      {
         return NULL;
      }
   }
   else
   {
      FIXGroups* grps = (FIXGroups*)fix_tag->data;
      FIXGroups* new_grps = fix_msg_realloc(msg, fix_tag->data, sizeof(FIXGroups) + sizeof(FIXGroup*) * (fix_tag->size + 1));
      memcpy(new_grps->group, grps->group, sizeof(FIXGroup*) * (fix_tag->size));
      new_grps->group[fix_tag->size] = fix_msg_alloc_group(msg);
      if (!new_grps->group[fix_tag->size])
      {
         return NULL;
      }
      ++fix_tag->size;
      fix_tag->data = new_grps;
      msg->body_len -= fix_tag->body_len;
   }
   if (LIKE(fix_tag->num != FIXTagNum_BeginString && fix_tag->num != FIXTagNum_BodyLength && fix_tag->num != FIXTagNum_CheckSum))
   {
      fix_tag->body_len = fix_utils_numdigits(tagNum) + 1 + fix_utils_numdigits(fix_tag->size) + 1;
   }
   msg->body_len += fix_tag->body_len;
   FIXGroups* grps = (FIXGroups*)fix_tag->data;
   FIXGroup* new_grp = grps->group[fix_tag->size - 1];
   *tag = fix_tag;
   return new_grp;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_group_get(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx)
{
   int32_t const idx = tagNum % GROUP_SIZE;
   FIXGroup* group = (grp ? grp : msg->tags);
   FIXTag* it = group->tags[idx];
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
         if (grpIdx >= it->size)
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
int32_t fix_group_del(FIXMsg* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx)
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
   fix_tag->size -= 1;
   if (fix_tag->size == grpIdx)
   {
      grps->group[fix_tag->size] = NULL;
   }
   else
   {
      memcpy((char*)grps->group + sizeof(FIXGroup*) * grpIdx, (char*)grps->group + sizeof(FIXGroup*) * (grpIdx + 1),
         sizeof(FIXGroup*) * (fix_tag->size - grpIdx));
      grps->group[fix_tag->size] = NULL;
   }
   if (!fix_tag->size) // all groups has been deleted, so tag will be deleted either
   {
      return fix_tag_del(msg, grp, tagNum);
   }
   else
   {
      msg->body_len -= fix_tag->body_len;
      fix_tag->body_len = fix_utils_numdigits(tagNum) + 1 + fix_utils_numdigits(fix_tag->size) + 1;
      msg->body_len += fix_tag->body_len;
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
      for(int32_t i = 0; i < fix_tag->size; ++i)
      {
         fix_group_free(msg, grps->group[i]);
      }
   }
   msg->body_len -= fix_tag->body_len;
   return fix_tag->next;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_group_free(FIXMsg* msg, FIXGroup* group)
{
   for(int32_t i = 0; i < GROUP_SIZE; ++i)
   {
      FIXTag* tag = group->tags[i];
      while(tag)
      {
         tag = fix_tag_free(msg, tag);
      }
   }
   fix_msg_free_group(msg, group);
}