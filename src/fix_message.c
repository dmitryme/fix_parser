/// @file   fix_message.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/30/2012 06:35:11 PM

#include "fix_message.h"
#include "fix_protocol_descr.h"
#include "fix_tag.h"
#include "fix_error.h"
#include "fix_mpool.h"
#include "fix_utils.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern void set_fix_error(int, char const*, ...);

struct FIXMessage_
{
   FIXMPool* pool;
   FIXMessageDescr* descr;
   FIXTagTable* tags;
   uint32_t flags;
};

FIXMessage* new_fix_message(FIXProtocolVerEnum ver, char const* msgType, uint32_t memSize, uint32_t flags)
{
   reset_fix_error();
   FIXProtocolDescr* prot = get_fix_protocol_descr(ver);
   if (!prot)
   {
     return NULL;
   }
   if (!msgType)
   {
      set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "MsgType parameter is empty");
      return NULL;
   }
   FIXMessage* msg = malloc(sizeof(FIXMessage));
   msg->pool = new_fix_mpool(memSize);
   msg->tags = new_fix_table(msg->pool);
   msg->descr = get_fix_message_descr(prot, msgType);
   msg->flags = flags;
   if (!msg->descr)
   {
     free_fix_message(msg);
     return NULL;
   }
   return msg;
}

void free_fix_message(FIXMessage* msg)
{
   free_fix_mpool(msg->pool);
   free_fix_table(msg->tags);
   free(msg);
}

FIXTag* set_tag(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned char const* data, uint32_t len)
{
   reset_fix_error();
   if (!msg)
   {
      set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Msg parameter is empty");
      return NULL;
   }
   if (grp)
   {
      return set_fix_table_tag(grp, tagNum, data, len);
   }
   else
   {
      return set_fix_table_tag(msg->tags, tagNum, data, len);
   }
}

FIXTag* set_tag_fmt(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char const* fmt, ...)
{
   reset_fix_error();
   if (!msg)
   {
      set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Msg parameter is empty");
      return NULL;
   }

   int n, size = 64;
   char *p, *np;
   va_list ap;

   if ((p = malloc(size)) == NULL)
   {
      set_fix_error(FIX_ERROR_MALLOC, "Unable to allocate %d bytes", size);
      return NULL;
   }

   while (1)
   {
      va_start(ap, fmt);
      n = vsnprintf(p, size, fmt, ap);
      va_end(ap);

      if (n < 0)
      {
         set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Unable to set tag. Wrong format '%s'", fmt);
         return NULL;
      }

      if (n < size)
      {
         FIXTag* tag = NULL;
         if (grp)
         {
            tag = set_fix_table_tag(grp, tagNum, p, n);
         }
         else
         {
            tag = set_fix_table_tag(msg->tags, tagNum, p, n);
         }
         free(p);
         return tag;
      }

      if (size == n)
      {
         size *= 2;
      }

      if ((np = realloc (p, size)) == NULL)
      {
         free(p);
         set_fix_error(FIX_ERROR_MALLOC, "Unable to allocate %d bytes", size);
         return NULL;
      }
      else
      {
         p = np;
      }
   }
}

FIXTag* get_tag(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum)
{
   reset_fix_error();
   if (!msg)
   {
      set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Msg parameter is empty");
      return NULL;
   }
   if (grp)
   {
      return get_fix_table_tag(grp, tagNum);
   }
   else
   {
      return get_fix_table_tag(msg->tags, tagNum);
   }
}

int del_tag(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum)
{
   reset_fix_error();
   if (!msg)
   {
      set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Msg parameter is empty");
      return FIX_FAILED;
   }
   if (grp)
   {
      return del_fix_table_tag(grp, tagNum);
   }
   else
   {
      return del_fix_table_tag(msg->tags, tagNum);
   }
}

FIXTagTable* add_group(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum)
{
   reset_fix_error();
   if (!msg)
   {
      set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Msg parameter is empty");
      return NULL;
   }
   if (grp)
   {
      return add_fix_table_group(grp, tagNum);
   }
   else
   {
      return add_fix_table_group(msg->tags, tagNum);
   }
}

FIXTagTable* get_group(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, uint32_t grpIdx)
{
   reset_fix_error();
   if (!msg)
   {
      set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Msg parameter is empty");
      return NULL;
   }
   if (grp)
   {
      return get_fix_table_group(grp, tagNum, grpIdx);
   }
   else
   {
      return get_fix_table_group(msg->tags, tagNum, grpIdx);
   }
}

int del_group(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, uint32_t grpIdx)
{
   reset_fix_error();
   if (!msg)
   {
      set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Msg parameter is empty");
      return FIX_FAILED;
   }
   if (grp)
   {
      return del_fix_table_group(grp, tagNum, grpIdx);
   }
   else
   {
      return del_fix_table_group(msg->tags, tagNum, grpIdx);
   }
}

FIXTag* set_tag_string(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char const* val)
{
   reset_fix_error();
   FIXFieldDescr* fdescr = get_fix_field_descr(msg->descr, tagNum);
   if (!fdescr)
   {
     return NULL;
   }
   if (!IS_STRING_TYPE(fdescr->field_type->type))
   {
     set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value.", tagNum);
     return NULL;
   }
   return set_tag(msg, grp, tagNum, val, strlen(val));
}

FIXTag* set_tag_long(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, long val)
{
   reset_fix_error();
   FIXFieldDescr* fdescr = get_fix_field_descr(msg->descr, tagNum);
   if (!fdescr)
   {
     return NULL;
   }
   if (!IS_INT_TYPE(fdescr->field_type->type))
   {
     set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value.", tagNum);
     return NULL;
   }
   return set_tag(msg, grp, tagNum, (unsigned char*)&val, sizeof(val));
}

FIXTag* set_tag_ulong(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned long val)
{
   reset_fix_error();
   FIXFieldDescr* fdescr = get_fix_field_descr(msg->descr, tagNum);
   if (!fdescr)
   {
     return NULL;
   }
   if (!IS_INT_TYPE(fdescr->field_type->type))
   {
     set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value.", tagNum);
     return NULL;
   }
   return set_tag(msg, grp, tagNum, (unsigned char*)&val, sizeof(val));
}

FIXTag* set_tag_char(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char val)
{
   reset_fix_error();
   FIXFieldDescr* fdescr = get_fix_field_descr(msg->descr, tagNum);
   if (!fdescr)
   {
     return NULL;
   }
   if (!IS_CHAR_TYPE(fdescr->field_type->type))
   {
     set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value.", tagNum);
     return NULL;
   }
   return set_tag(msg, grp, tagNum, &val, 1);
}

FIXTag* set_tag_float(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, float val)
{
   reset_fix_error();
   FIXFieldDescr* fdescr = get_fix_field_descr(msg->descr, tagNum);
   if (!fdescr)
   {
     return NULL;
   }
   if (!IS_FLOAT_TYPE(fdescr->field_type->type))
   {
     set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value.", tagNum);
     return NULL;
   }
   return set_tag(msg, grp, tagNum, (unsigned char*)&val, sizeof(val));
}

int get_tag_long(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, long* val)
{
   reset_fix_error();
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   *val = *(long*)&tag->data;
   return FIX_SUCCESS;
}
int get_tag_ulong(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned long* val)
{
   reset_fix_error();
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   *val = *(unsigned long*)&tag->data;
   return FIX_SUCCESS;
}
int get_tag_float(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, float* val)
{
   reset_fix_error();
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   *val = *(float*)&tag->data;
   return FIX_SUCCESS;
}
int get_tag_char(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char* val)
{
   reset_fix_error();
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   *val = *(char*)&tag->data;
   return FIX_SUCCESS;
}
int get_tag_string(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char* val, uint32_t len)
{
   reset_fix_error();
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   strncpy(val, (char const*)&tag->data, len);
   return tag->size;
}