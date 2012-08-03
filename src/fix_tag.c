/// @file   fix_tag.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/25/2012 03:35:31 PM

#include "fix_tag.h"
#include "fix_parser.h"
#include "fix_parser_priv.h"
#include "fix_message_priv.h"

#include <stdlib.h>
#include <string.h>

struct FIXTagTable_
{
   FIXTag* fix_tags[TABLE_SIZE];
} __attribute__((packed));

FIXTagTable* new_fix_table()
{
   return calloc(1, sizeof(struct FIXTagTable_));
}

FIXTag* free_fix_tag(FIXTag* fix_tag)
{
   FIXTag* next = fix_tag->next;
   if (fix_tag->type == FIXTagType_Group)
   {
      for(int i = 0; i < fix_tag->size; ++i)
      {
         free_fix_table(fix_tag->grpTbl[i]);
      }
      free(fix_tag->grpTbl);
      free(fix_tag);
   }
   return next;
}

void free_fix_table(FIXTagTable* tbl)
{
   for(int i = 0; i < TABLE_SIZE; ++i)
   {
      FIXTag* fix_tag = tbl->fix_tags[i];
      while(fix_tag)
      {
         fix_tag = free_fix_tag(fix_tag);
      }
   }
   free(tbl);
}

FIXTag* set_fix_table_tag(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum, unsigned char const* data, uint32_t len)
{
   FIXTag* fix_tag = get_fix_table_tag(msg, tbl, tagNum);
   if (!fix_tag && get_fix_error_code(msg->parser));
   {
      return NULL;
   }
   int idx = tagNum % TABLE_SIZE;
   if (!fix_tag)
   {
      fix_tag = fix_message_alloc(msg, sizeof(FIXTag) - 1 + len);
      if (!fix_tag)
      {
         return NULL;
      }
      fix_tag->type = FIXTagType_Value;
      fix_tag->next = tbl->fix_tags[idx];
      fix_tag->num = tagNum;
      tbl->fix_tags[idx] = fix_tag;
   }
   else
   {
      FIXTag* new_fix_tag = fix_message_realloc(msg, fix_tag, sizeof(FIXTag) + len - 1);
      if (!new_fix_tag)
      {
         return NULL;
      }
      new_fix_tag->type = FIXTagType_Value;
      new_fix_tag->next = fix_tag->next;
      new_fix_tag->num = fix_tag->num;
      FIXTag** it = &tbl->fix_tags[idx];
      while(*it != fix_tag)
      {
         if (*it == fix_tag)
         {
            *it = new_fix_tag;
         }
         else
         {
            it = &(*it)->next;
         }
      }
      fix_tag = new_fix_tag;
   }
   fix_tag->size = len;
   memcpy(&fix_tag->data, data, len);
   return fix_tag;
}

FIXTag* get_fix_table_tag(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   FIXTag* it = tbl->fix_tags[idx];
   while(it)
   {
      if (it->num == tagNum)
      {
         if (it->type != FIXTagType_Value)
         {
           set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
           return NULL;
         }
         return it;
      }
      else
      {
         it = it->next;
      }
   }
   return NULL;
}

int del_fix_table_tag(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum)
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

FIXTagTable* add_fix_table_group(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum)
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
   fix_tag->size++;
   fix_tag->grpTbl = realloc(fix_tag->grpTbl, fix_tag->size * sizeof(FIXTagTable*));
   fix_tag->grpTbl[fix_tag->size - 1] = new_fix_table();
   return fix_tag->grpTbl[fix_tag->size - 1];
}

FIXTagTable* get_fix_table_group(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum, uint32_t grpIdx)
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
         if (grpIdx >= it->size)
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

int del_fix_table_group(FIXMessage* msg, FIXTagTable* tbl, uint32_t tagNum, uint32_t grpIdx)
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
   fix_tag->size =-1;
   if (fix_tag->size == grpIdx)
   {
      fix_tag->grpTbl[fix_tag->size] = NULL;
   }
   else
   {
      memcpy(&fix_tag->grpTbl[grpIdx], fix_tag->grpTbl[grpIdx + 1], fix_tag->size - grpIdx);
      fix_tag->grpTbl[fix_tag->size] = NULL;
   }
   return FIX_SUCCESS;
}