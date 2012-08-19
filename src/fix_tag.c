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

FIXTag* free_fix_tag(FIXTag* fix_tag)
{
   FIXTag* next = fix_tag->next;
   if (fix_tag->type == FIXTagType_Group)
   {
      uint32_t grp_count = *(uint32_t*)&fix_tag->data;
      for(int i = 0; i < grp_count; ++i)
      {
         /* TODO */
         /*FIXGroup* grp = fix_tag->data*/
         /*fix_parser_free_group()*/
         /*free_fix_table(fix_tag->grpTbl[i]);*/
      }
      free(fix_tag);
   }
   return next;
}

/* TODO */
/*void free_fix_group(FIXGroup* grp)*/
/*{*/
/*   for(int i = 0; i < TABLE_SIZE; ++i)*/
/*   {*/
/*      FIXTag* fix_tag = grp->fix_tags[i];*/
/*      while(fix_tag)*/
/*      {*/
/*         fix_tag = free_fix_tag(fix_tag);*/
/*      }*/
/*   }*/
/*   fix_parser_free_group(grp);*/
/*}*/

FIXTag* set_fix_table_tag(FIXMessage* msg, FIXGroup* tbl, uint32_t tagNum, unsigned char const* data, uint32_t len)
{
   FIXTag* fix_tag = get_fix_table_tag(msg, tbl, tagNum);
   if (!fix_tag && get_fix_error_code(msg->parser))
   {
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
      fix_tag->next = tbl->fix_tags[idx];
      fix_tag->num = tagNum;
      tbl->fix_tags[idx] = fix_tag;
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

FIXTag* get_fix_table_tag(FIXMessage* msg, FIXGroup* tbl, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   FIXTag* it = tbl->fix_tags[idx];
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

int del_fix_table_tag(FIXMessage* msg, FIXGroup* tbl, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   FIXTag* fix_tag = tbl->fix_tags[idx];
   FIXTag* prev = fix_tag;
   while(fix_tag)
   {
      if (fix_tag->num == tagNum)
      {
         if (prev == fix_tag)
         {
            free_fix_tag(fix_tag);
            tbl->fix_tags[idx] = NULL;
         }
         else
         {
            prev->next = free_fix_tag(fix_tag);
         }
         return FIX_SUCCESS;
      }
      if (prev == fix_tag)
      {
         fix_tag = fix_tag->next;
      }
      else
      {
         prev = fix_tag;
         fix_tag = fix_tag->next;
      }
   }
   set_fix_error(msg->parser, FIX_ERROR_TAG_NOT_FOUND, "FIXTag not found");
   return FIX_FAILED;
}
/*
FIXGroup* add_fix_table_group(FIXMessage* msg, FIXGroup* tbl, uint32_t tagNum)
{
   FIXTag* fix_tag = get_fix_table_tag(msg, tbl, tagNum);
   if (fix_tag && fix_tag->type != FIXTagType_Group)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
      return NULL;
   }
   if (!fix_tag)
   {
      uint32_t const idx = tagNum % TABLE_SIZE;
      fix_tag = calloc(1, sizeof(FIXTag));
      fix_tag->type = FIXTagType_Group;
      fix_tag->num = tagNum;
      fix_tag->next = tbl->fix_tags[idx];
      tbl->fix_tags[idx] = fix_tag;
   }
   uint32_t grp_count = ++*(uint32_t*)&fix_tag->data;
   fix_tag->data = realloc(&fix_tag->data + sizeof(uint32_t), grp_count * sizeof(FIXGroup*));
   fix_tag->grpTbl[fix_tag->grp_count - 1] = new_fix_table();
   return fix_tag->grpTbl[fix_tag->grp_count - 1];
}

FIXGroup* get_fix_table_group(FIXMessage* msg, FIXGroup* tbl, uint32_t tagNum, uint32_t grpIdx)
{
   int const idx = tagNum % TABLE_SIZE;
   FIXTag* it = tbl->fix_tags[idx];
   while(it)
   {
      if (it->num == tagNum)
      {
         if (it->type != FIXTagType_Group)
         {
            set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
            return NULL;
         }
         if (grpIdx >= it->grp_count)
         {
            set_fix_error(msg->parser, FIX_ERROR_GROUP_WRONG_INDEX, "Wrong index");
            return NULL;
         }
         return it->grpTbl[grpIdx];
      }
      else
      {
         it = it->next;
      }
   }
   return NULL;
}

int del_fix_table_group(FIXMessage* msg, FIXGroup* tbl, uint32_t tagNum, uint32_t grpIdx)
{
   FIXTag* fix_tag = get_fix_table_tag(msg, tbl, tagNum);
   if (!fix_tag)
   {
      return FIX_FAILED;
   }
   if (fix_tag->type != FIXTagType_Group)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
      return FIX_FAILED;
   }
   free_fix_table(fix_tag->grpTbl[grpIdx]);
   fix_tag->grp_count =-1;
   if (fix_tag->grp_count == grpIdx)
   {
      fix_tag->grpTbl[fix_tag->grp_count] = NULL;
   }
   else
   {
      memcpy(&fix_tag->grpTbl[grpIdx], fix_tag->grpTbl[grpIdx + 1], fix_tag->grp_count - grpIdx);
      fix_tag->grpTbl[fix_tag->grp_count] = NULL;
   }
   return FIX_SUCCESS;
}  */
