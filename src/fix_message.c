/// @file   fix_message.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/30/2012 06:35:11 PM

#include "fix_message.h"
#include "fix_protocol_descr.h"
#include "fix_tag.h"
#include "fix_error.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern void set_fix_error(int, char const*, ...);

FIXMessage* create_fix_message(FIXProtocolVerEnum ver, char const* msgType, uint32_t flags)
{
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
   msg->tags = new_fix_table(fix_mpool_init(1024));
   msg->descr = get_fix_message_descr(prot, msgType);
   msg->flags = flags;
   if (!msg->descr)
   {
      return NULL;
   }
   return msg;
}

void free_fix_message(FIXMessage* msg)
{

}

FIXTag* set_tag(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned char const* data, uint32_t len)
{
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
   if (!msg)
   {
      set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Msg parameter is empty");
      return NULL;
   }

   int n, size = 100;
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
   return set_tag(msg, grp, tagNum, val, strlen(val));
}

FIXTag* set_tag_long(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, long val)
{
   return set_tag_fmt(msg, grp, tagNum, "%ld", val);
}

FIXTag* set_tag_ulong(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned long val)
{
   return set_tag_fmt(msg, grp, tagNum, "%lu", val);
}

FIXTag* set_tag_char(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char val)
{
   return set_tag_fmt(msg, grp, tagNum, &val, 1);
}

FIXTag* set_tag_float(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, float val)
{
   return set_tag_fmt(msg, grp, tagNum, "%f", val);
}

int get_tag_long(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, long* val)
{
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
      return FIX_FAILED;
   }
   if (tag->type != FIXTagType_Value)
   {
      set_fix_error(FIX_ERROR_TAG_HAS_WRONG_TYPE, "Tag %d is not a value", tagNum);
      return FIX_FAILED;
   }
   /**val = strtol(tag->value.data, tag->value.data + tag->value.len, 10);*/
   return FIX_SUCCESS;
}
int get_tag_ulong(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned long* val)
{
   *val = 0;
   return FIX_SUCCESS;
}
int get_tag_float(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, float* val)
{
   *val = 0.0;
   return FIX_SUCCESS;
}
int get_tag_char(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char* val)
{
   *val = 'A';
   return FIX_SUCCESS;
}
int get_tag_string(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char* val)
{
   *val = 0;
   return FIX_SUCCESS;
}