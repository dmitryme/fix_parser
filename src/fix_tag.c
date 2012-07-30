/// @file   tag.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/25/2012 03:35:31 PM

#include "tag.h"
#include "error.h"

#include <stdlib.h>
#include <string.h>

extern void set_fix_error(int code, char const* text, ...);

struct TagTable_
{
   Tag* tags[TABLE_SIZE];
};

TagTable* new_table()
{
   return (TagTable*)calloc(1, sizeof(struct TagTable_));
}

Tag* free_tag(Tag* tag)
{
   Tag* next = tag->next;
   if (tag->type == TagValue)
   {
      free(tag->value.data);
      free(tag);
   }
   else if (tag->type == TagGroup)
   {
      for(int i = 0; i < tag->groups.size; ++i)
      {
         free_table(tag->groups.grpTbl[i]);
      }
      free(tag->groups.grpTbl);
      free(tag);
   }
   return next;
}

void free_table(TagTable* tbl)
{
   for(int i = 0; i < TABLE_SIZE; ++i)
   {
      Tag* tag = tbl->tags[i];
      while(tag)
      {
         tag = free_tag(tag);
      }
   }
   free(tbl);
}

Tag* set_tag(TagTable* tbl, uint32_t tagNum, unsigned char const* data, uint32_t len)
{
   Tag* tag = get_tag(tbl, tagNum);
   if (!tag && get_fix_last_error()->code != FIX_ERROR_TAG_NOT_FOUND)
   {
      return NULL;
   }
   if (!tag)
   {
      int idx = tagNum % TABLE_SIZE;
      tag = malloc(sizeof(Tag));
      tag->type = TagValue;
      tag->next = tbl->tags[idx];
      tag->num = tagNum;
      tbl->tags[idx] = tag;
   }
   if (tag->value.data)
   {
      free(tag->value.data);
   }
   tag->value.data = (unsigned char*)malloc(len);
   tag->value.len = len;
   memcpy(tag->value.data, data, len);
   return tag;
}

Tag* get_tag(TagTable* tbl, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   Tag* it = tbl->tags[idx];
   while(it)
   {
      if (it->num == tagNum)
      {
         if (it->type != TagValue)
         {
            set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag has wrong type");
            return NULL;
         }
         return it;
      }
      else
      {
         it = it->next;
      }
   }
   set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "Tag not found");
   return NULL;
}

int rm_tag(TagTable* tbl, uint32_t tagNum)
{
   uint32_t const idx = tagNum % TABLE_SIZE;
   Tag* tag = tbl->tags[idx];
   Tag* prev = tag;
   while(tag)
   {
      if (tag->num == tagNum)
      {
         if (prev == tag)
         {
            free_tag(tag);
            tbl->tags[idx] = NULL;
         }
         else
         {
            prev->next = free_tag(tag);
         }
         return FIX_SUCCESS;
      }
      if (prev == tag)
      {
         tag = tag->next;
      }
      else
      {
         prev = tag;
         tag = tag->next;
      }
   }
   set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "Tag not found");
   return FIX_FAILED;
}

TagTable* add_group(TagTable* tbl, uint32_t tagNum)
{
   Tag* tag = get_tag(tbl, tagNum);
   if (tag && tag->type != TagGroup)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag has wrong type");
      return NULL;
   }
   if (!tag)
   {
      uint32_t const idx = tagNum % TABLE_SIZE;
      tag = calloc(1, sizeof(Tag));
      tag->type = TagGroup;
      tag->num = tagNum;
      tag->next = tbl->tags[idx];
      tbl->tags[idx] = tag;
   }
   tag->groups.size++;
   tag->groups.grpTbl = realloc(tag->groups.grpTbl, tag->groups.size * sizeof(TagTable*));
   tag->groups.grpTbl[tag->groups.size - 1] = new_table();
   return tag->groups.grpTbl[tag->groups.size - 1];
}

TagTable* get_grp(TagTable* tbl, uint32_t tagNum, uint32_t grpIdx)
{
   int const idx = tagNum % TABLE_SIZE;
   Tag* it = tbl->tags[idx];
   while(it)
   {
      if (it->num == tagNum)
      {
         if (it->type != TagGroup)
         {
            set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag has wrong type");
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
   set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "Tag not found");
   return NULL;
}

int rm_grp(TagTable* tbl, uint32_t tagNum, uint32_t grpIdx)
{
   Tag* tag = get_tag(tbl, tagNum);
   if (!tag)
   {
      return FIX_FAILED;
   }
   if (tag->type != TagGroup)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag has wrong type");
      return FIX_FAILED;
   }
   free_table(tag->groups.grpTbl[grpIdx]);
   tag->groups.size =-1;
   if (tag->groups.size == grpIdx)
   {
      tag->groups.grpTbl[tag->groups.size] = NULL;
   }
   else
   {
      memcpy(&tag->groups.grpTbl[grpIdx], tag->groups.grpTbl[grpIdx + 1], tag->groups.size - grpIdx);
      tag->groups.grpTbl[tag->groups.size] = NULL;
   }
   return FIX_SUCCESS;
}