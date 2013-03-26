/**
 * @file   fix_msg.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 06:35:11 PM
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

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXMsg* fix_msg_create(FIXParser* parser, char const* msgType, FIXError** error)
{
   if (!parser)
   {
      return NULL;
   }
   if (!msgType)
   {
      *error = fix_error_create(FIX_ERROR_INVALID_ARGUMENT, "MsgType parameter is NULL");
      return NULL;
   }
   FIXMsgDescr const* msg_descr = fix_protocol_get_msg_descr(parser, msgType, error);
   if (!msg_descr)
   {
      return NULL;
   }
   FIXMsg* msg = (FIXMsg*)malloc(sizeof(FIXMsg));
   msg->fields = msg->used_groups = fix_parser_alloc_group(parser, error);
   if (!msg->fields)
   {
      fix_msg_free(msg);
      return NULL;
   }
   msg->descr = msg_descr;
   msg->parser = parser;
   msg->pages = msg->curr_page = fix_parser_alloc_page(parser, 0, error);
   if (!msg->pages)
   {
      fix_msg_free(msg);
      return NULL;
   }
   msg->body_len = 0;
   fix_msg_set_string(msg, NULL, 8, parser->protocol->transportVersion, error);
   fix_msg_set_string(msg, NULL, 35, msgType, error);
   return msg;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API void fix_msg_free(FIXMsg* msg)
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
char const* fix_msg_get_type(FIXMsg const* msg)
{
   if (!msg)
   {
      return NULL;
   }
   return msg->descr->type;
}

/*------------------------------------------------------------------------------------------------------------------------*/
char const* fix_msg_get_name(FIXMsg const* msg)
{
   if (!msg)
   {
      return NULL;
   }
   return msg->descr->name;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXGroup* fix_msg_add_group(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, FIXError** error)
{
   if (!msg)
   {
      return NULL;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return NULL;
   }
   if (fdescr->category != FIXFieldCategory_Group)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Field '%d' is not a group", tag);
      return NULL;
   }

   FIXField* field = NULL;
   FIXGroup* new_grp = fix_group_add(msg, grp, fdescr, &field, error);
   new_grp->parent_fdescr = fdescr;
   return new_grp;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXGroup* fix_msg_get_group(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, uint32_t grpIdx, FIXError** error)
{
   if (!msg)
   {
      return NULL;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return NULL;
   }
   if (fdescr->category != FIXFieldCategory_Group)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' is not a group tag", tag);
      return NULL;
   }
   return fix_group_get(msg, grp, tag, grpIdx, error);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_del_group(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, uint32_t grpIdx, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (fdescr->category != FIXFieldCategory_Group)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' is not a group tag", tag);
      return FIX_FAILED;
   }
   return fix_group_del(msg, grp, tag, grpIdx, error);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_set_string(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, char const* val, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_STRING_TYPE(fdescr->type->valueType))
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatible with value '%s'", tag, val);
      return FIX_FAILED;
   }
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)val, strlen(val), error);
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_set_int32(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, int32_t val, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_INT_TYPE(fdescr->type->valueType))
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%d'", tag, val);
      return FIX_FAILED;
   }
   char buff[64] = {};
   int32_t res = fix_utils_i64toa(val, buff, sizeof(buff), 0);
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)buff, res, error);
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_set_int64(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, int64_t val, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_INT_TYPE(fdescr->type->valueType))
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%ld'", tag, val);
      return FIX_FAILED;
   }
   char buff[64] = {};
   int32_t res = fix_utils_i64toa(val, buff, sizeof(buff), 0);
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)buff, res, error);
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_set_char(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, char val, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_CHAR_TYPE(fdescr->type->valueType))
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%c'", tag, val);
      return FIX_FAILED;
   }
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)&val, 1, error);
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_set_double(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, double val, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_FLOAT_TYPE(fdescr->type->valueType))
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatrible with value '%f'", tag, val);
      return FIX_FAILED;
   }
   char buff[64] = {};
   int32_t res = fix_utils_dtoa(val, buff, sizeof(buff));
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)buff, res, error);
   return field != NULL ? FIX_SUCCESS : FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_set_data(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, char const* data, uint32_t dataLen,
      FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (!IS_DATA_TYPE(fdescr->type->valueType))
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag '%d' type is not compatible with data value", tag);
      return FIX_FAILED;
   }
   FIXField* field = fix_msg_set_field(msg, grp, fdescr, (unsigned char*)data, dataLen, error);
   if (!field)
   {
      return FIX_FAILED;
   }
   return fix_msg_set_int32(msg, grp, fdescr->dataLenField->type->tag, dataLen, error);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_get_int32(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, int32_t* val, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag %d is not a value", tag);
      return FIX_FAILED;
   }
   int32_t cnt;
   return fix_utils_atoi32((char const*)field->data, field->size, 0, val, &cnt);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_get_int64(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, int64_t* val, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Field %d is not a value", tag);
      return FIX_FAILED;
   }
   int32_t cnt;
   return fix_utils_atoi64((char const*)field->data, field->size, 0, val, &cnt);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_get_double(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, double* val, FIXError** error)
{
   if(!msg)
   {
      return FIX_FAILED;
   }
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Field %d is not a value", tag);
      return FIX_FAILED;
   }
   int32_t cnt;
   return fix_utils_atod((char const*)field->data, field->size, 0, val, &cnt);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_get_char(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, char* val, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Tag %d is not a value", tag);
      return FIX_FAILED;
   }
   *val = *(char*)(field->data);
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_get_string(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, char const** val, uint32_t* len,
      FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Value)
   {
      *error = fix_error_create(FIX_ERROR_FIELD_HAS_WRONG_TYPE, "Field %d is not a value", tag);
      return FIX_FAILED;
   }
   *val = (char const*)field->data;
   *len = field->size;
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_get_data(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, char const** val, uint32_t* len,
      FIXError** error)
{
   return fix_msg_get_string(msg, grp, tag, val, len, error);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_del_field(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, FIXError** error)
{
   if (!msg)
   {
      return FIX_FAILED;
   }
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      fix_error_create(FIX_ERROR_FIELD_NOT_FOUND, "Field '%d' not found", tag);
      return FIX_FAILED;
   }
   FIXFieldDescr const* fdescr = fix_protocol_get_descr(msg, grp, tag, error);
   if (!fdescr)
   {
      return FIX_FAILED;
   }
   if (IS_DATA_TYPE(fdescr->type->valueType))
   {
      fix_field_del(msg, grp, fdescr->dataLenField->type->tag, error);
   }
   return fix_field_del(msg, grp, tag, error);
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_msg_to_str(FIXMsg* msg, char delimiter, char* buff, uint32_t buffLen, uint32_t* reqBuffLen,
      FIXError** error)
{
   if(!msg || !msg || !reqBuffLen)
   {
      return FIX_FAILED;
   }
   FIXMsgDescr const* descr = msg->descr;
   int32_t crc = 0;
   uint32_t buffLenBefore = buffLen;
   for(uint32_t i = 0; i < descr->field_count; ++i)
   {
      char* prev = buff;
      FIXFieldDescr* fdescr = &descr->fields[i];
      FIXField* field = fix_field_get(msg, NULL, fdescr->type->tag);
      FIXErrCode res = FIX_SUCCESS;
      if (fdescr->type->tag == FIXFieldTag_BodyLength)
      {
         res = int32_to_str(fdescr->type->tag, msg->body_len, delimiter, 0, 0, &buff, &buffLen, error);
         *reqBuffLen = buffLenBefore - buffLen + msg->body_len + 7;
         if (*reqBuffLen > buffLenBefore)
         {
            return FIX_FAILED;
         }
         crc += (FIX_SOH - delimiter); // adjust CRC, if delimiter is not equal to FIX_SOH
      }
      else if(fdescr->type->tag == FIXFieldTag_CheckSum)
      {
         res = int32_to_str(fdescr->type->tag, crc % 256, delimiter, 3, '0', &buff, &buffLen, error);
      }
      else if ((msg->parser->flags & PARSER_FLAG_CHECK_REQUIRED) && !field && (fdescr->flags & FIELD_FLAG_REQUIRED))
      {
         *error = fix_error_create(FIX_ERROR_FIELD_NOT_FOUND, "Tag '%d' is required", fdescr->type->tag);
         return FIX_FAILED;
      }
      else if (field && field->descr->category == FIXFieldCategory_Group)
      {
         res = fix_groups_to_string(msg, field, fdescr, delimiter, &buff, &buffLen, &crc, error);
      }
      else if(field)
      {
         if (msg->parser->flags & PARSER_FLAG_CHECK_VALUE)
         {
            if (!fix_protocol_check_field_value(fdescr, field->data, field->size))
            {
               *error = fix_error_create(FIX_ERROR_WRONG_FIELD_VALUE, "Wrong field '%s' value.", fdescr->type->name);
               return FIX_FAILED;
            }
         }
         res = field_to_str(field, delimiter, &buff, &buffLen, error);
         crc += (FIX_SOH - delimiter); // adjust CRC, if delimiter is not equal to FIX_SOH
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
   return FIX_SUCCESS;
}
