/// @file   fix_fix_tag.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/25/2012 03:35:31 PM

#include "fix_tag.h"
#include "fix_error.h"

#include <stdlib.h>
#include <string.h>

extern void set_fix_error(int code, char const* text, ...);

struct FIXTagTable_
{
   FIXTag* fix_tags[TABLE_SIZE];
};

FIXTagTable* new_fix_table()
{
   return (FIXTagTable*)calloc(1, sizeof(struct FIXTagTable_));
}

FIXTag* free_fix_tag(FIXTag* fix_tag)
{
   FIXTag* next = fix_tag->next;
   if (fix_tag->type == FIXTagType_Value)
   {
      free(fix_tag->value.data);
      free(fix_tag);
   }
   else if (fix_tag->type == FIXTagType_Group)
   {
      for(int i = 0; i < fix_tag->groups.size; ++i)
      {
         free_fix_table(fix_tag->groups.grpTbl[i]);
      }
      free(fix_tag->groups.grpTbl);
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

FIXTag* set_fix_table_tag(FIXTagTable* tbl, uint32_t tagNum, unsigned char const* data, uint32_t len)
{
   FIXTag* fix_tag = get_fix_table_tag(tbl, tagNum);
   if (!fix_tag && get_fix_last_error()->code != FIX_ERROR_TAG_NOT_FOUND)
   {
     return NULL;
   }
   if (!fix_tag)
   {
     int idx = tagNum % TABLE_SIZE;
     fix_tag = calloc(1, sizeof(FIXTag));
     fix_tag->type = FIXTagType_Value;
     fix_tag->next = tbl->fix_tags[idx];
     fix_tag->num = tagNum;
     tbl->fix_tags[idx] = fix_tag;
   }
   if (fix_tag->value.data)
   {
     free(fix_tag->value.data);
   }
   fix_tag->value.data = (unsigned char*)malloc(len);
   fix_tag->value.len = len;
   memcpy(fix_tag->value.data, data, len);
   return fix_tag;
}

FIXTag* get_fix_table_tag(FIXTagTable* tbl, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   FIXTag* it = tbl->fix_tags[idx];
   while(it)
   {
      if (it->num == tagNum)
      {
         if (it->type != FIXTagType_Value)
         {
            set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
            return NULL;
         }
         return it;
      }
      else
      {
         it = it->next;
      }
   }
   set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "FIXTag not found");
   return NULL;
}

int del_fix_table_tag(FIXTagTable* tbl, uint32_t tagNum)
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
   set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "FIXTag not found");
   return FIX_FAILED;
}

FIXTagTable* add_fix_table_group(FIXTagTable* tbl, uint32_t tagNum)
{
   FIXTag* fix_tag = get_fix_table_tag(tbl, tagNum);
   if (fix_tag && fix_tag->type != FIXTagType_Group)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
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
   fix_tag->groups.size++;
   fix_tag->groups.grpTbl = realloc(fix_tag->groups.grpTbl, fix_tag->groups.size * sizeof(FIXTagTable*));
   fix_tag->groups.grpTbl[fix_tag->groups.size - 1] = new_fix_table();
   return fix_tag->groups.grpTbl[fix_tag->groups.size - 1];
}

FIXTagTable* get_fix_table_group(FIXTagTable* tbl, uint32_t tagNum, uint32_t grpIdx)
{
   int const idx = tagNum % TABLE_SIZE;
   FIXTag* it = tbl->fix_tags[idx];
   while(it)
   {
      if (it->num == tagNum)
      {
         if (it->type != FIXTagType_Group)
         {
            set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
            return NULL;
         }
         if (grpIdx >= it->groups.size)
         {
            set_fix_error(FIX_ERROR_GROUP_WRONG_INDEX, "Wrong index");
            return NULL;
         }
         return it->groups.grpTbl[grpIdx];
      }
      else
      {
         it = it->next;
      }
   }
   set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "FIXTag not found");
   return NULL;
}

int del_fix_table_group(FIXTagTable* tbl, uint32_t tagNum, uint32_t grpIdx)
{
   FIXTag* fix_tag = get_fix_table_tag(tbl, tagNum);
   if (!fix_tag)
   {
      return FIX_FAILED;
   }
   if (fix_tag->type != FIXTagType_Group)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "FIXTag has wrong type");
      return FIX_FAILED;
   }
   free_fix_table(fix_tag->groups.grpTbl[grpIdx]);
   fix_tag->groups.size =-1;
   if (fix_tag->groups.size == grpIdx)
   {
      fix_tag->groups.grpTbl[fix_tag->groups.size] = NULL;
   }
   else
   {
      memcpy(&fix_tag->groups.grpTbl[grpIdx], fix_tag->groups.grpTbl[grpIdx + 1], fix_tag->groups.size - grpIdx);
      fix_tag->groups.grpTbl[fix_tag->groups.size] = NULL;
   }
   return FIX_SUCCESS;
}