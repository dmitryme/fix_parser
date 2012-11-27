/* @file   fix_parser.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:26:54 AM
*/

#include "fix_parser.h"
#include "fix_parser_priv.h"
#include "fix_protocol_descr.h"
#include "fix_msg.h"
#include "fix_msg_priv.h"
#include "fix_page.h"
#include "fix_utils.h"
#include "fix_error_priv.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CRC_FIELD_LEN 7

/*------------------------------------------------------------------------------------------------------------------------*/
FIXParser* fix_parser_create(char const* protFile, FIXParserAttrs const* attrs, int32_t flags)
{
   fix_static_error_reset();
   FIXParserAttrs myattrs = {};
   if (attrs)
   {
      memcpy(&myattrs, attrs, sizeof(myattrs));
   }
   if (!fix_parser_validate_attrs(&myattrs))
   {
      return NULL;
   }
   FIXParser* parser = calloc(1, sizeof(FIXParser));
   memcpy(&parser->attrs, &myattrs, sizeof(parser->attrs));
   parser->flags = flags;
   parser->protocol = fix_protocol_descr_create(fix_static_error_get(), protFile);
   if (!parser->protocol)
   {
      return NULL;
   }
   for(uint32_t i = 0; i < parser->attrs.numPages; ++i)
   {
      FIXPage* page = calloc(1, sizeof(FIXPage) + parser->attrs.pageSize - 1);
      page->size = parser->attrs.pageSize;
      page->next = parser->page;
      parser->page = page;
   }
   for(uint32_t i = 0; i < parser->attrs.numGroups; ++i)
   {
      FIXGroup* group = calloc(1, sizeof(FIXGroup));
      group->next = parser->group;
      parser->group = group;
   }
   return parser;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_parser_free(FIXParser* parser)
{
   if (parser)
   {
      if (parser->protocol)
      {
         fix_protocol_descr_free(parser->protocol);
      }
      FIXPage* page = parser->page;
      while(page)
      {
         FIXPage* next = page->next;
         free(page);
         page = next;
      }
      FIXGroup* group = parser->group;
      while(group)
      {
         FIXGroup* next = group->next;
         free(group);
         group = next;
      }
      free(parser);
   }
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXMsg* fix_parser_fix_to_msg(FIXParser* parser, char const* data, uint32_t len, char delimiter, char const** stop)
{
   if (!parser || !data)
   {
      return NULL;
   }
   fix_error_reset(&parser->error);
   int64_t tag = 0;
   char const* dbegin = NULL;
   char const* dend = NULL;
   tag = fix_parser_parse_field(parser, data, len, delimiter, &dbegin, &dend);
   if (tag == FIX_FAILED)
   {
      fix_error_set(&parser->error, FIX_ERROR_PARSE_MSG, "Unable to parse BeginString field.");
      return NULL;
   }
   if (tag != FIXFieldTag_BeginString)
   {
      fix_error_set(&parser->error, FIX_ERROR_WRONG_FIELD, "First field is '%d', but must be BeginString.", tag);
      return NULL;
   }
   if (strncmp(parser->protocol->transportVersion, dbegin, dend - dbegin))
   {
      char* actualVer = calloc(dend - dbegin + 1, 1);
      memcpy(actualVer, dbegin, dend - dbegin);
      fix_error_set(
            &parser->error,
            FIX_ERROR_WRONG_PROTOCOL_VER,
            "Wrong protocol. Expected '%s', actual '%s'.", parser->protocol->transportVersion, actualVer);
      free(actualVer);
      return NULL;
   }
   tag = fix_parser_parse_field(parser, dend + 1, len - (dend - data - 1), delimiter, &dbegin, &dend);
   if (tag == FIX_FAILED)
   {
      fix_error_set(&parser->error, FIX_ERROR_PARSE_MSG, "Unable to parse BodyLength field.");
      return NULL;
   }
   if (tag != FIXFieldTag_BodyLength)
   {
      fix_error_set(&parser->error, FIX_ERROR_WRONG_FIELD, "Second field is '%d', but must be BodyLength.", tag);
      return NULL;
   }
   int64_t bodyLen;
   if (fix_utils_atoi64(dbegin, dend - dbegin, 0, &bodyLen) == FIX_FAILED)
   {
      fix_error_set(&parser->error, FIX_ERROR_PARSE_MSG, "BodyLength value not a tagber.");
      return NULL;
   }
   if (bodyLen + CRC_FIELD_LEN > len - (dend - data))
   {
      return NULL;
   }
   char const* crcbeg = NULL;
   char const* bodyEnd = dend + bodyLen;
   tag = fix_parser_parse_field(parser, bodyEnd + 1, CRC_FIELD_LEN, delimiter, &crcbeg, stop);
   if (tag == FIX_FAILED)
   {
      fix_error_set(&parser->error, FIX_ERROR_PARSE_MSG, "Unable to parse CrcSum field.");
      return NULL;
   }
   if (tag != FIXFieldTag_CheckSum)
   {
      fix_error_set(&parser->error, FIX_ERROR_WRONG_FIELD, "Field is '%d', but must be CrcSum.", tag);
      return NULL;
   }
   if (parser->flags & PARSER_FLAG_CHECK_CRC)
   {
      int64_t check_sum = 0;
      if (fix_utils_atoi64(crcbeg, *stop - crcbeg, 0, &check_sum) == FIX_FAILED)
      {
         fix_error_set(&parser->error, FIX_ERROR_PARSE_MSG, "CheckSum value not a tagber.");
         return NULL;
      }
      int64_t crc = 0;
      for(char const* it = data; it <= bodyEnd; ++it)
      {
         crc += *it;
      }
      crc %= 256;
      if (crc != check_sum)
      {
         fix_error_set(
               &parser->error,
               FIX_ERROR_INTEGRITY_CHECK, "CheckSum check failed. Expected '%d', actual '%d'.", check_sum, crc);
         return NULL;
      }
   }
   tag = fix_parser_parse_field(parser, dend + 1, bodyEnd - dend - 1, delimiter, &dbegin, &dend);
   if (tag == FIX_FAILED)
   {
      fix_error_set(&parser->error, FIX_ERROR_PARSE_MSG, "Unable to parse MsgType field.");
      return NULL;
   }
   if (tag != FIXFieldTag_MsgType)
   {
      fix_error_set(&parser->error, FIX_ERROR_WRONG_FIELD, "Field is '%d', but must be MsgType.", tag);
      return NULL;
   }
   char* msgType = calloc(dend - dbegin + 1, 1);
   memcpy(msgType, dbegin, dend - dbegin);
   FIXMsg* msg = fix_msg_create(parser, "8");
   if (!msg)
   {
      free(msgType);
      return NULL;
   }
   free(msgType);
   while(dend != bodyEnd)
   {
      tag = fix_parser_parse_field(parser, dend + 1, bodyEnd - dend, delimiter, &dbegin, &dend);
      if (tag == FIX_FAILED)
      {
         fix_msg_free(msg);
         fix_error_set(&parser->error, FIX_ERROR_PARSE_MSG, "Unable to parse MsgType field.");
         return NULL;
      }
      FIXFieldDescr* fdescr = fix_protocol_get_field_descr(&parser->error, msg->descr, tag);
      if (!fdescr)
      {
         if (parser->flags & PARSER_FLAG_CHECK_UNKNOWN_FIELDS)
         {
            fix_msg_free(msg);
            fix_error_set(&parser->error, FIX_ERROR_UNKNOWN_FIELD, "Field '%d' not found in description.", tag);
            return NULL;
         }
      }
      else
      {
         if (parser->flags & PARSER_FLAG_CHECK_VALUE)
         {
            if (fix_parser_check_value(fdescr, dbegin, dend, delimiter) == FIX_FAILED)
            {
               fix_msg_free(msg);
               return NULL;
            }
         }
         if (fdescr->category == FIXFieldCategory_Group)
         {
            assert(0);
            int64_t num = 0;
            int32_t res = fix_utils_atoi64(dbegin, dend - dbegin, delimiter, &num);
            if (res == FIX_FAILED)
            {
               fix_msg_free(msg);
               fix_error_set(&parser->error, FIX_ERROR_INVALID_ARGUMENT, "Unable to get group tag %d value.", tag);
               return NULL;
            }
            res = fix_parser_parse_group(msg, num, dend + 1, bodyEnd - dend, delimiter, &dend);
            if (res == FIX_FAILED)
            {
               fix_msg_free(msg);
               return NULL;
            }
         }
         fix_field_set(msg, NULL, fdescr, (unsigned char*)dbegin, dend - dbegin);
      }
   }
   return msg;
}
