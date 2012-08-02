/// @file   fix_message.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/30/2012 06:35:11 PM

#include "fix_message.h"
#include "fix_parser.h"
#include "fix_protocol_descr.h"
#include "fix_tag.h"
#include "fix_mpool.h"
#include "fix_types.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern void set_fix_error(FIXParser*, int, char const*, ...);
extern void reset_fix_error(FIXParser*);
extern FIXProtocolDescr* get_fix_protocol_descr(FIXParser* parser, FIXProtocolVerEnum ver);

struct FIXMessage_
{
   FIXParser* parser;
   FIXMPool* pool;
   FIXMessageDescr* descr;
   FIXTagTable* tags;
};

FIXMessage* new_fix_message(FIXParser* parser, FIXProtocolVerEnum ver, char const* msgType)
{
   if (!parser)
   {
      return NULL;
   }
   reset_fix_error(parser);
   if (!msgType)
   {
      set_fix_error(parser, FIX_ERROR_INVALID_ARGUMENT, "MsgType parameter is NULL");
      return NULL;
   }
   FIXProtocolDescr* prot = get_fix_protocol_descr(parser, ver);
   if (!prot)
   {
      return NULL;
   }
   FIXMessageDescr* msg_descr = get_fix_message_descr(parser, prot, msgType);
   if (!msg_descr)
   {
      return NULL;
   }
   FIXMessage* msg = malloc(sizeof(FIXMessage));
   msg->pool = new_fix_mpool();
   msg->tags = new_fix_table();
   msg->descr = msg_descr;
   msg->parser = parser;
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
   if (grp)
   {
      return set_fix_table_tag(msg->parser, msg->pool, grp, tagNum, data, len);
   }
   else
   {
      return set_fix_table_tag(msg->parser, msg->pool, msg->tags, tagNum, data, len);
   }
}

FIXTag* set_tag_fmt(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char const* fmt, ...)
{
   int n, size = 64;
   char *p, *np;
   va_list ap;

   if ((p = malloc(size)) == NULL)
   {
      set_fix_error(msg->parser, FIX_ERROR_MALLOC, "Unable to allocate %d bytes", size);
      return NULL;
   }

   while (1)
   {
      va_start(ap, fmt);
      n = vsnprintf(p, size, fmt, ap);
      va_end(ap);

      if (n < 0)
      {
         set_fix_error(msg->parser, FIX_ERROR_INVALID_ARGUMENT, "Unable to set tag. Wrong format '%s'", fmt);
         return NULL;
      }

      if (n < size)
      {
         FIXTag* tag = NULL;
         if (grp)
         {
            tag = set_fix_table_tag(msg->parser, msg->pool, grp, tagNum, (unsigned char*)p, n);
         }
         else
         {
            tag = set_fix_table_tag(msg->parser, msg->pool, msg->tags, tagNum, (unsigned char*)p, n);
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
         set_fix_error(msg->parser, FIX_ERROR_MALLOC, "Unable to allocate %d bytes", size);
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
   if (!msg)
   {
      return NULL;
   }
   reset_fix_error(msg->parser);
   if (grp)
   {
      return get_fix_table_tag(msg->parser, grp, tagNum);
   }
   else
   {
      return get_fix_table_tag(msg->parser, msg->tags, tagNum);
   }
}

int del_tag(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   reset_fix_error(msg->parser);
   if (grp)
   {
      return del_fix_table_tag(msg->parser, grp, tagNum);
   }
   else
   {
      return del_fix_table_tag(msg->parser, msg->tags, tagNum);
   }
}

FIXTagTable* add_group(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum)
{
   if (!msg)
   {
      return NULL;
   }
   reset_fix_error(msg->parser);
   if (get_fix_parser_flags(msg->parser) & FIX_PARSER_FLAGS_VALIDATE)
   {
      FIXFieldDescr* fdescr = get_fix_field_descr(msg->parser, msg->descr, tagNum);
      if (!fdescr)
      {
         return NULL;
      }
   }
   if (grp)
   {
      return add_fix_table_group(msg->parser, grp, tagNum);
   }
   else
   {
      return add_fix_table_group(msg->parser, msg->tags, tagNum);
   }
}

FIXTagTable* get_group(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, uint32_t grpIdx)
{
   if (!msg)
   {
      return NULL;
   }
   reset_fix_error(msg->parser);
   if (grp)
   {
      return get_fix_table_group(msg->parser, grp, tagNum, grpIdx);
   }
   else
   {
      return get_fix_table_group(msg->parser, msg->tags, tagNum, grpIdx);
   }
}

int del_group(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, uint32_t grpIdx)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   reset_fix_error(msg->parser);
   if (grp)
   {
      return del_fix_table_group(msg->parser, grp, tagNum, grpIdx);
   }
   else
   {
      return del_fix_table_group(msg->parser, msg->tags, tagNum, grpIdx);
   }
}

FIXTag* set_tag_string(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char const* val)
{
   if (!msg)
   {
      return NULL;
   }
   reset_fix_error(msg->parser);
   if (get_fix_parser_flags(msg->parser) * FIX_PARSER_FLAGS_VALIDATE)
   {
      FIXFieldDescr* fdescr = get_fix_field_descr(msg->parser, msg->descr, tagNum);
      if (!fdescr)
      {
         return NULL;
      }
      if (!IS_STRING_TYPE(fdescr->field_type->type))
      {
         set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%s'", tagNum, val);
         return NULL;
      }
   }
   return set_tag(msg, grp, tagNum, (unsigned char*)val, strlen(val));
}

FIXTag* set_tag_long(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, long val)
{
   if (!msg)
   {
      return NULL;
   }
   reset_fix_error(msg->parser);
   if (get_fix_parser_flags(msg->parser) & FIX_PARSER_FLAGS_VALIDATE)
   {
      FIXFieldDescr* fdescr = get_fix_field_descr(msg->parser, msg->descr, tagNum);
      if (!fdescr)
      {
         return NULL;
      }
      if (!IS_INT_TYPE(fdescr->field_type->type))
      {
         set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%ld'", tagNum, val);
         return NULL;
      }
   }
   return set_tag(msg, grp, tagNum, (unsigned char*)&val, sizeof(val));
}

FIXTag* set_tag_ulong(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned long val)
{
   if (!msg)
   {
      return NULL;
   }
   reset_fix_error(msg->parser);
   if (get_fix_parser_flags(msg->parser) & FIX_PARSER_FLAGS_VALIDATE)
   {
      FIXFieldDescr* fdescr = get_fix_field_descr(msg->parser, msg->descr, tagNum);
      if (!fdescr)
      {
         return NULL;
      }
      if (!IS_INT_TYPE(fdescr->field_type->type))
      {
         set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%lu'", tagNum, val);
         return NULL;
      }
   }
   return set_tag(msg, grp, tagNum, (unsigned char*)&val, sizeof(val));
}

FIXTag* set_tag_char(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char val)
{
   if (!msg)
   {
      return NULL;
   }
   reset_fix_error(msg->parser);
   if (get_fix_parser_flags(msg->parser) & FIX_PARSER_FLAGS_VALIDATE)
   {
      FIXFieldDescr* fdescr = get_fix_field_descr(msg->parser, msg->descr, tagNum);
      if (!fdescr)
      {
         return NULL;
      }
      if (!IS_CHAR_TYPE(fdescr->field_type->type))
      {
         set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%c'", tagNum, val);
         return NULL;
      }
   }
   return set_tag(msg, grp, tagNum, (unsigned char*)&val, 1);
}

FIXTag* set_tag_float(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, float val)
{
   if (!msg)
   {
      return NULL;
   }
   reset_fix_error(msg->parser);
   if (get_fix_parser_flags(msg->parser) & FIX_PARSER_FLAGS_VALIDATE)
   {
      FIXFieldDescr* fdescr = get_fix_field_descr(msg->parser, msg->descr, tagNum);
      if (!fdescr)
      {
         return NULL;
      }
      if (!IS_FLOAT_TYPE(fdescr->field_type->type))
      {
         set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%f'", tagNum, val);
         return NULL;
      }
   }
   return set_tag(msg, grp, tagNum, (unsigned char*)&val, sizeof(val));
}

int get_tag_long(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, long* val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   reset_fix_error(msg->parser);
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(msg->parser, grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->parser, msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   *val = *(long*)&tag->data;
   return FIX_SUCCESS;
}
int get_tag_ulong(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned long* val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   reset_fix_error(msg->parser);
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(msg->parser, grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->parser, msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   *val = *(unsigned long*)&tag->data;
   return FIX_SUCCESS;
}
int get_tag_float(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, float* val)
{
   if(!msg)
   {
      return FIX_FAILED;
   }
   reset_fix_error(msg->parser);
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(msg->parser, grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->parser, msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   *val = *(float*)&tag->data;
   return FIX_SUCCESS;
}
int get_tag_char(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char* val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   reset_fix_error(msg->parser);
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(msg->parser, grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->parser, msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   *val = *(char*)&tag->data;
   return FIX_SUCCESS;
}
int get_tag_string(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char* val, uint32_t len)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   reset_fix_error(msg->parser);
   FIXTag* tag = NULL;
   if (grp)
   {
      tag = get_fix_table_tag(msg->parser, grp, tagNum);
   }
   else
   {
      tag = get_fix_table_tag(msg->parser, msg->tags, tagNum);
   }
   if (!tag)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_NOT_FOUND, "Tag '%d' not found", tagNum);
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(msg->parser, FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   strncpy(val, (char const*)&tag->data, len);
   return tag->size;
}