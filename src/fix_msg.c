/* @file   fix_msg.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 06:35:11 PM
*/

#include "fix_msg.h"
#include "fix_msg_priv.h"
#include "fix_parser.h"
#include "fix_parser_priv.h"
#include "fix_protocol_descr.h"
#include "fix_field.h"
#include "fix_types.h"
#include "fix_page.h"
#include "fix_utils.h"
#include "fix_error.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/*-----------------------------------------------------------------------------------------------------------------------*/
/* PUBLICS                                                                                                               */
/*-----------------------------------------------------------------------------------------------------------------------*/
FIXMsg* fix_msg_create(FIXParser* parser, char const* msgType)
{
   if (!parser)
   {
      return NULL;
   }
   fix_error_reset(&parser->error);
   if (!msgType)
   {
      fix_error_set(&parser->error, FIX_ERROR_INVALID_ARGUMENT, "MsgType parameter is NULL");
      return NULL;
   }
   FIXMsgDescr* msg_descr = fix_protocol_get_msg_descr(parser, msgType);
   if (!msg_descr)
   {
      return NULL;
   }
   FIXMsg* msg = malloc(sizeof(FIXMsg));
   msg->fields = msg->used_groups = fix_parser_alloc_group(parser);
   if (!msg->fields)
   {
      fix_msg_free(msg);
      return NULL;
   }
   msg->descr = msg_descr;
   msg->parser = parser;
   msg->pages = msg->curr_page = fix_parser_alloc_page(parser, 0);
   if (!msg->pages)
   {
      fix_msg_free(msg);
      return NULL;
   }
   msg->body_len = 0;
   fix_msg_set_string(msg, NULL, 8, parser->protocol->transportVersion);
   fix_msg_set_string(msg, NULL, 35, msgType);
   return msg;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_msg_free(FIXMsg* msg)
{
   if (!msg)
   {
      return;
   }
   FIXPage* page = msg->pages;
   while(page)
   {
      page = fix_parser_free_page(msg->parser, page);
   }
   FIXGroup* grp = msg->used_groups;
   while(grp)
   {
      grp = fix_parser_free_group(msg->parser, grp);

   }
   free(msg);
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
FIXField* fix_msg_get_field(FIXMsg* msg, FIXGroup* grp, uint32_t tag)
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
int32_t fix_msg_del_field(FIXMsg* msg, FIXGroup* grp, uint32_t tag)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   return fix_field_del(msg, grp, tag);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_msg_add_group(FIXMsg* msg, FIXGroup* grp, uint32_t tag)
{
   if (!msg)
   {
      return NULL;
   }
   fix_error_reset(&msg->parser->error);
   FIXFieldDescr* fdescr = NULL;

   fdescr = fix_protocol_get_field_descr(&msg->parser->error, msg->descr, tag);
   if (!fdescr)
   {
      return NULL;
   }
   if (fdescr->category != FIXFieldCategory_Group)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Field '%d' is not a group", tag);
      return NULL;
   }

   FIXField* field = NULL;
   FIXGroup* new_grp = fix_group_add(msg, grp, fdescr, &field);
   new_grp->parent_fdescr = fdescr;
   return new_grp;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_msg_get_group(FIXMsg* msg, FIXGroup* grp, uint32_t tag, uint32_t grpIdx)
{
   if (!msg)
   {
      return NULL;
   }
   fix_error_reset(&msg->parser->error);
   FIXFieldDescr* fdescr = fix_protocol_get_field_descr(&msg->parser->error, msg->descr, tag);
   if (!fdescr)
   {
      return NULL;
   }
   if (fdescr->category != FIXFieldCategory_Group)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' is not a group tag", tag);
      return NULL;
   }
   return fix_group_get(msg, grp, tag, grpIdx);
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_del_group(FIXMsg* msg, FIXGroup* grp, uint32_t tag, uint32_t grpIdx)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXFieldDescr* fdescr = fix_protocol_get_field_descr(&msg->parser->error, msg->descr, tag);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (fdescr->category != FIXFieldCategory_Group)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' is not a group tag", tag);
      return FIX_FAILED;
   }
   return fix_group_del(msg, grp, tag, grpIdx);
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_set_string(FIXMsg* msg, FIXGroup* grp, uint32_t tag, char const* val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXFieldDescr* fdescr = grp ? fix_protocol_get_group_descr(&msg->parser->error, grp->parent_fdescr, tag) :
      fix_protocol_get_field_descr(&msg->parser->error, msg->descr, tag);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_STRING_TYPE(fdescr->type->valueType))
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatible with value '%s'", tag, val);
      return FIX_FAILED;
   }
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)val, strlen(val));
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_set_int32(FIXMsg* msg, FIXGroup* grp, uint32_t tag, int32_t val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXFieldDescr* fdescr = grp ? fix_protocol_get_group_descr(&msg->parser->error, grp->parent_fdescr, tag) :
      fix_protocol_get_field_descr(&msg->parser->error, msg->descr, tag);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_INT_TYPE(fdescr->type->valueType))
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%d'", tag, val);
      return FIX_FAILED;
   }
   char buff[64] = {};
   int32_t res = fix_utils_i64toa(val, buff, sizeof(buff), 0);
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)buff, res);
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_set_int64(FIXMsg* msg, FIXGroup* grp, uint32_t tag, int64_t val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXFieldDescr* fdescr = grp ? fix_protocol_get_group_descr(&msg->parser->error, grp->parent_fdescr, tag) :
      fix_protocol_get_field_descr(&msg->parser->error, msg->descr, tag);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_INT_TYPE(fdescr->type->valueType))
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%ld'", tag, val);
      return FIX_FAILED;
   }
   char buff[64] = {};
   int32_t res = fix_utils_i64toa(val, buff, sizeof(buff), 0);
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)buff, res);
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_set_char(FIXMsg* msg, FIXGroup* grp, uint32_t tag, char val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXFieldDescr* fdescr = grp ? fix_protocol_get_group_descr(&msg->parser->error, grp->parent_fdescr, tag) :
      fix_protocol_get_field_descr(&msg->parser->error, msg->descr, tag);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_CHAR_TYPE(fdescr->type->valueType))
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%c'", tag, val);
      return FIX_FAILED;
   }
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)&val, 1);
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_set_double(FIXMsg* msg, FIXGroup* grp, uint32_t tag, double val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXFieldDescr* fdescr = grp ? fix_protocol_get_group_descr(&msg->parser->error, grp->parent_fdescr, tag) :
      fix_protocol_get_field_descr(&msg->parser->error, msg->descr, tag);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_FLOAT_TYPE(fdescr->type->valueType))
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%f'", tag, val);
      return FIX_FAILED;
   }
   char buff[64] = {};
   int32_t res = fix_utils_dtoa(val, buff, sizeof(buff));
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)buff, res);
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_get_int32(FIXMsg* msg, FIXGroup* grp, uint32_t tag, int32_t* val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag %d is not a value", tag);
      return FIX_FAILED;
   }
   if (field->size > sizeof(int32_t))
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_INVALID_ARGUMENT, "Data is too long (%d bytes)", field->size);
      return FIX_FAILED;
   }
   return fix_utils_atoi64(field->data, field->size, 0, (int64_t*)val) > 0 ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_get_int64(FIXMsg* msg, FIXGroup* grp, uint32_t tag, int64_t* val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Field %d is not a value", tag);
      return FIX_FAILED;
   }
   return fix_utils_atoi64(field->data, field->size, 0, val) > 0 ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_get_double(FIXMsg* msg, FIXGroup* grp, uint32_t tag, double* val)
{
   if(!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Field %d is not a value", tag);
      return FIX_FAILED;
   }
   return fix_utils_atod(field->data, field->size, 0, val) > 0 ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_get_char(FIXMsg* msg, FIXGroup* grp, uint32_t tag, char* val)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag %d is not a value", tag);
      return FIX_FAILED;
   }
   *val = *(char*)(field->data);
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_get_string(FIXMsg* msg, FIXGroup* grp, uint32_t tag, char const** val, uint32_t* len)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Field %d is not a value", tag);
      return FIX_FAILED;
   }
   *val = (char const*)field->data;
   *len = field->size;
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_msg_to_string(FIXMsg* msg, char delimiter, char* buff, uint32_t buffLen)
{
   if(!msg)
   {
      return FIX_FAILED;
   }
   fix_error_reset(&msg->parser->error);
   FIXMsgDescr* descr = msg->descr;
   uint32_t crc = 0;
   int32_t buffLenBefore = buffLen;
   for(uint32_t i = 0; i < descr->field_count; ++i)
   {
      char* prev = buff;
      FIXFieldDescr* fdescr = &descr->fields[i];
      FIXField* field = fix_field_get(msg, NULL, fdescr->type->tag);
      int32_t res = FIX_SUCCESS;
      if (fdescr->type->tag == FIXFieldTag_BodyLength)
      {
         res = int32_to_fix_msg(msg->parser, fdescr->type->tag, msg->body_len, delimiter, 0, 0, &buff, &buffLen);
      }
      else if(fdescr->type->tag == FIXFieldTag_CheckSum)
      {
         res = int32_to_fix_msg(msg->parser, fdescr->type->tag, crc % 256, delimiter, 3, '0', &buff, &buffLen);
      }
      else if ((msg->parser->flags & PARSER_FLAG_CHECK_REQUIRED) && !field && (fdescr->flags & FIELD_FLAG_REQUIRED))
      {
         fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_NOT_FOUND, "Tag '%d' is required", fdescr->type->tag);
         return FIX_FAILED;
      }
      else if (field && field->descr->category == FIXFieldCategory_Group)
      {
         res = fix_groups_to_string(msg, field, fdescr, delimiter, &buff, &buffLen);
      }
      else if(field)
      {
         res = fix_field_to_fix_msg(msg->parser, field, delimiter, &buff, &buffLen);
      }
      if (res == FIX_FAILED)
      {
         return FIX_FAILED;
      }
      for(;(field || fdescr->type->tag == FIXFieldTag_BodyLength) && prev != buff; ++prev)
      {
         crc += *prev;
      }
   }
   return buffLenBefore - buffLen;
}

/*------------------------------------------------------------------------------------------------------------------------*/
/* PRIVATES                                                                                                               */
/*------------------------------------------------------------------------------------------------------------------------*/

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
int32_t int32_to_fix_msg(FIXParser* parser, uint32_t tag, int32_t val, char delimiter, uint32_t width, char padSym, char** buff, uint32_t* buffLen)
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
int32_t fix_field_to_fix_msg(FIXParser* parser, FIXField* field, char delimiter, char** buff, uint32_t* buffLen)
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
int32_t fix_groups_to_string(FIXMsg* msg, FIXField* field, FIXFieldDescr* fdescr, char delimiter, char** buff, uint32_t* buffLen)
{
   int32_t res = int32_to_fix_msg(msg->parser, field->descr->type->tag, field->size, delimiter, 0, 0, buff, buffLen);
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