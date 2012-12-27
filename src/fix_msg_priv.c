/* @file   fix_msg.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 06:35:11 PM
*/

#include "fix_msg_priv.h"
#include "fix_parser_priv.h"
#include "fix_utils.h"

#include <string.h>

/*-----------------------------------------------------------------------------------------------------------------------*/
void* fix_msg_alloc(FIXMsg* msg, uint32_t size)
{
   FIXPage* curr_page = msg->curr_page;
   if (sizeof(uint32_t) + curr_page->size - curr_page->offset >= size)
   {
      uint32_t old_offset = curr_page->offset;
      *(uint32_t*)(&curr_page->data + curr_page->offset) = size;
      curr_page->offset += (size + sizeof(uint32_t));
      return &curr_page->data + old_offset + sizeof(uint32_t);
   }
   else
   {
      FIXPage* new_page = fix_parser_alloc_page(msg->parser, size);
      if (!new_page)
      {
         return NULL;
      }
      curr_page->next = new_page;
      msg->curr_page = new_page;
      return fix_msg_alloc(msg, size);
   }
}

/*------------------------------------------------------------------------------------------------------------------------*/
void* fix_msg_realloc(FIXMsg* msg, void* ptr, uint32_t size)
{
   if (*(uint32_t*)(ptr - sizeof(uint32_t)) >= size)
   {
      return ptr;
   }
   else
   {
      return fix_msg_alloc(msg, size);
   }
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXField* fix_msg_set_field(FIXMsg* msg, FIXGroup* grp, FIXFieldDescr* fdescr, unsigned char const* data, uint32_t len)
{
   if (grp)
   {
      return fix_field_set(msg, grp, fdescr, data, len);
   }
   else
   {
      return fix_field_set(msg, msg->fields, fdescr, data, len);
   }
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_msg_alloc_group(FIXMsg* msg)
{
   FIXGroup* grp = fix_parser_alloc_group(msg->parser);
   if (grp)
   {
      grp->next = msg->used_groups;
      msg->used_groups = grp;
   }
   return grp;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_msg_free_group(FIXMsg* msg, FIXGroup* grp)
{
   FIXGroup* curr_grp = msg->used_groups;
   FIXGroup* prev_grp = msg->used_groups;
   while(curr_grp)
   {
      if (curr_grp == grp)
      {
         if (curr_grp == prev_grp)
         {
            msg->used_groups = grp->next;
         }
         else
         {
            prev_grp->next = curr_grp->next;
         }
         fix_parser_free_group(msg->parser, grp);
         break;
      }
      prev_grp = curr_grp;
      curr_grp = curr_grp->next;
   }
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXField* fix_msg_get_field(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag)
{
   if (!msg)
   {
      return NULL;
   }
   fix_error_reset(&msg->parser->error);
   if (grp)
   {
      return fix_field_get(msg, grp, tag);
   }
   else
   {
      return fix_field_get(msg, msg->fields, tag);
   }
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_msg_del_field(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   return fix_field_del(msg, grp, tag);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode int32_to_fix_msg(FIXParser* parser, FIXTagNum tag, int32_t val, char delimiter, uint32_t width, char padSym, char** buff, uint32_t* buffLen)
{
   if (UNLIKE(*buffLen == 0))
   {
      fix_error_set(&parser->error, FIX_ERROR_NO_MORE_SPACE, "Not enough buffer space.");
      return FIX_FAILED;
   }
   int32_t res = fix_utils_i64toa(tag, *buff, *buffLen, 0);
   *buff += res;
   *buffLen -= res;
   if (UNLIKE(*buffLen == 0))
   {
      fix_error_set(&parser->error, FIX_ERROR_NO_MORE_SPACE, "Not enough buffer space.");
      return FIX_FAILED;
   }
   *(*buff) = '=';
   *buff += 1;
   *buffLen -= 1;
   if (UNLIKE(*buffLen == 0))
   {
      fix_error_set(&parser->error, FIX_ERROR_NO_MORE_SPACE, "Not enough buffer space.");
      return FIX_FAILED;
   }
   res = fix_utils_i64toa(val, *buff, (width == 0) ? *buffLen : width, padSym);
   *buff += res;
   *buffLen -= res;
   if (UNLIKE(*buffLen == 0))
   {
      fix_error_set(&parser->error, FIX_ERROR_NO_MORE_SPACE, "Not enough buffer space.");
      return FIX_FAILED;
   }
   *(*buff) = delimiter;
   *buff += 1;
   *buffLen -= 1;
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_field_to_fix_msg(FIXParser* parser, FIXField* field, char delimiter, char** buff, uint32_t* buffLen)
{
   if (UNLIKE(*buffLen == 0))
   {
      fix_error_set(&parser->error, FIX_ERROR_NO_MORE_SPACE, "Not enough buffer space.");
      return FIX_FAILED;
   }
   int32_t res = fix_utils_i64toa(field->descr->type->tag, *buff, *buffLen, 0);
   *buff += res;
   *buffLen -= res;
   if (UNLIKE(*buffLen == 0))
   {
      fix_error_set(&parser->error, FIX_ERROR_NO_MORE_SPACE, "Not enough buffer space.");
      return FIX_FAILED;
   }
   *(*buff) = '=';
   *buff += 1;
   *buffLen -= 1;
   if (UNLIKE(*buffLen == 0))
   {
      fix_error_set(&parser->error, FIX_ERROR_NO_MORE_SPACE, "Not enough buffer space.");
      return FIX_FAILED;
   }
   uint32_t const len = (*buffLen > field->size) ? field->size : *buffLen;
   strncpy(*buff, field->data, len);
   *buff += len;
   *buffLen -= len;
   if (UNLIKE(*buffLen == 0))
   {
      fix_error_set(&parser->error, FIX_ERROR_NO_MORE_SPACE, "Not enough buffer space.");
      return FIX_FAILED;
   }
   *(*buff) = delimiter;
   *buff += 1;
   *buffLen -= 1;
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_groups_to_string(FIXMsg* msg, FIXField* field, FIXFieldDescr* fdescr, char delimiter, char** buff, uint32_t* buffLen)
{
   FIXErrCode res = int32_to_fix_msg(msg->parser, field->descr->type->tag, field->size, delimiter, 0, 0, buff, buffLen);
   for(uint32_t i = 0; i < field->size && res != FIX_FAILED; ++i)
   {
      FIXGroup* group = ((FIXGroups*)field->data)->group[i];
      for(uint32_t i = 0; i < fdescr->group_count && res == FIX_SUCCESS; ++i)
      {
         FIXFieldDescr* child_fdescr = &fdescr->group[i];
         FIXField* child_field = fix_field_get(msg, group, child_fdescr->type->tag);
         if ((msg->parser->flags & PARSER_FLAG_CHECK_REQUIRED) && !child_field && (child_fdescr->flags & FIELD_FLAG_REQUIRED))
         {
            fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' is required", child_fdescr->type->tag);
            return FIX_FAILED;
         }
         else if (!child_field && i == 0)
         {
            fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' must be first field in group", child_fdescr->type->tag);
            return FIX_FAILED;
         }
         else if (child_field && child_field->descr->category == FIXFieldCategory_Group)
         {
            res = fix_groups_to_string(msg, child_field, child_fdescr, delimiter, buff, buffLen);
         }
         else if(child_field)
         {
            res = fix_field_to_fix_msg(msg->parser, child_field, delimiter, buff, buffLen);
         }
      }
   }
   return FIX_SUCCESS;
}
