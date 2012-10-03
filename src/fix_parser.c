/* @file   fix_parser.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:26:54 AM
*/

#include "fix_parser.h"
#include "fix_parser_priv.h"
#include "fix_protocol_descr.h"
#include "fix_msg.h"
#include "fix_page.h"
#include "fix_utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define CRC_FIELD_LEN 7

/*------------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC                                                                                                                 */
/*------------------------------------------------------------------------------------------------------------------------*/

FIXParser* fix_parser_create(
      uint32_t pageSize, uint32_t maxPageSize, uint32_t numPages, uint32_t maxPages,
      uint32_t numGroups, uint32_t maxGroups, int32_t flags)
{
   FIXParser* parser = calloc(1, sizeof(FIXParser));
   parser->flags = flags;
   parser->page_size = pageSize;
   parser->max_page_size = maxPageSize;
   for(uint32_t i = 0; i < numPages; ++i)
   {
      FIXPage* page = calloc(1, sizeof(FIXPage) + pageSize - 1);
      page->size = pageSize;
      page->next = parser->page;
      parser->page = page;
   }
   parser->used_pages = 0;
   parser->max_pages = maxPages;
   for(uint32_t i = 0; i < numGroups; ++i)
   {
      FIXGroup* group = calloc(1, sizeof(FIXGroup));
      group->next = parser->group;
      parser->group = group;
   }
   parser->used_groups = 0;
   parser->max_groups = maxGroups;
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
int32_t get_fix_error_code(FIXParser* parser)
{
   if (parser)
   {
      return parser->err_code;
   }
   return FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
char const* get_fix_error_text(FIXParser* parser)
{
   if (parser)
   {
      return parser->err_text;
   }
   return NULL;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_protocol_init(FIXParser* parser, char const* protFile)
{
   if(!parser)
   {
      return FIX_FAILED;
   }
   FIXProtocolDescr* p = fix_protocol_descr_create(parser, protFile);
   if (!p)
   {
      return FIX_FAILED;
   }
   if (parser->protocol)
   {
      fix_protocol_descr_free(parser->protocol);
   }
   parser->protocol = p;
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE                                                                                                                */
/*------------------------------------------------------------------------------------------------------------------------*/
FIXPage* fix_parser_alloc_page(FIXParser* parser, uint32_t pageSize)
{
   if (parser->max_pages > 0 && parser->max_pages == parser->used_pages)
   {
      fix_parser_set_error(parser,
         FIX_ERROR_NO_MORE_PAGES, "No more pages available. MaxPages = %d, UsedPages = %d", parser->max_pages, parser->used_pages);
      return NULL;
   }
   FIXPage* page = NULL;
   if (parser->page == NULL) // no more free pages
   {
      uint32_t psize = (parser->page_size > pageSize ? parser->page_size : pageSize);
      if (parser->max_page_size > 0 && psize > parser->max_page_size)
      {
         fix_parser_set_error(
               parser,
               FIX_ERROR_TOO_BIG_PAGE, "Requested new page is too big. MaxPageSize = %d, RequestedPageSize = %d",
               parser->max_page_size, psize);
         return NULL;
      }
      page = calloc(1, sizeof(FIXPage) + psize - 1);
      page->size = psize;
   }
   else
   {
      page = parser->page;
      parser->page = page->next;
      page->next = NULL; // detach from pool of free pages
   }
   ++parser->used_pages;
   return page;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXPage* fix_parser_free_page(FIXParser* parser, FIXPage* page)
{
   FIXPage* next = page->next;
   page->offset = 0;
   page->next = parser->page;
   parser->page = page;
   --parser->used_pages;
   return next;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_parser_alloc_group(FIXParser* parser)
{
   if (parser->max_groups > 0 && parser->max_groups == parser->used_groups)
   {
      fix_parser_set_error(parser,
         FIX_ERROR_NO_MORE_GROUPS,
         "No more groups available. MaxGroups = %d, UsedGroups = %d", parser->max_groups, parser->used_groups);
      return NULL;
   }
   FIXGroup* group = NULL;
   if (parser->group == NULL) // no more free group
   {
      group = calloc(1, sizeof(FIXGroup));
   }
   else
   {
      group = parser->group;
      parser->group = group->next;
      group->next = NULL; // detach from pool
   }
   ++parser->used_groups;
   return group;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_parser_free_group(FIXParser* parser, FIXGroup* group)
{
   FIXGroup* next = group->next;
   memset(group, 0, sizeof(FIXGroup));
   group->next = parser->group;
   parser->group = group;
   --parser->used_groups;
   return next;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_parser_set_va_error(FIXParser* parser, int32_t code, char const* text, va_list ap)
{
   parser->err_code = code;
   int32_t n = vsnprintf(parser->err_text, ERROR_TXT_SIZE, text, ap);
   parser->err_text[n] = 0;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_parser_set_error(FIXParser* parser, int32_t code, char const* text, ...)
{
   va_list ap;
   va_start(ap, text);
   fix_parser_set_va_error(parser, code, text, ap);
   va_end(ap);
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_parser_reset_error(FIXParser* parser)
{
   parser->err_code = 0;
   parser->err_text[0] = 0;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int64_t parse_field(FIXParser* parser, char const* data, uint32_t len, char delimiter, char const** dbegin, char const** dend)
{
   int64_t num = 0;
   int32_t res = fix_utils_atoi64(data, len, '=', &num);
   if (res == FIX_FAILED)
   {
      fix_parser_set_error(parser, FIX_ERROR_INVALID_ARGUMENT, "Unable to extract field number.");
      return FIX_FAILED;
   }
   len -= res;
   *dend = *dbegin = data + res + 1;
   for(;len > 0; --len)
   {
      if (**dend == delimiter)
      {
         break;
      }
      else
      {
         ++(*dend);
      }
   }
   if (!len)
   {
      fix_parser_set_error(parser, FIX_ERROR_INVALID_ARGUMENT, "Field value must be terminated with delimiter.");
      return FIX_FAILED;
   }
   return num;
}

/*   FIXProtocolVerEnum mver = str2FIXProtocolVerEnum(dbegin, dend);*/
/*   num = parse_field(parser, dend + 1, len, &dbegin, &dend);*/
/*   if (res == FIX_FAILED)*/
/*   {*/
/*      //TODO:*/
/*      return FIX_FAILED;*/
/*   }*/
/*   id (num != FIXTagNum_MsgType)*/
/*   {*/
/*      //TODO:*/
/*      return FIX_FAILED;*/
/*   }*/
/*   res = parse_field(parser, data, len, &num, value);*/
/*   if (res == FIX_FAILED)*/
/*   {*/
/*      return FIX_FAILED;*/
/*   }*/
/*   res = fix_utils_atoi64(value, res, 0, bodyLen);*/
/*   if (res == FIX_FAILED)*/
/*   {*/
/*      return FIX_FAILED;*/
/*   }*/
/*}*/

/*------------------------------------------------------------------------------------------------------------------------*/
FIXMsg* parse_fix(FIXParser* parser, char const* data, uint32_t len, char delimiter, char const** stop)
{
   if (!parser || !data)
   {
      return NULL;
   }
   fix_parser_reset_error(parser);
   uint64_t tag = 0;
   char const* dbegin = NULL;
   char const* dend = NULL;
   tag = parse_field(parser, data, len, delimiter, &dbegin, &dend);
   if (tag == FIX_FAILED)
   {
      fix_parser_set_error(parser,FIX_ERROR_PARSE_MSG, "Unable to parse BeginString field.");
      return NULL;
   }
   if (tag != FIXFieldTag_BeginString)
   {
      fix_parser_set_error(parser,FIX_ERROR_WRONG_FIELD, "First field is '%d', but must be BeginString.", tag);
      return NULL;
   }
   if (strncmp(parser->protocol->transportVersion, dbegin, dend - dbegin))
   {
      char* actualVer = calloc(dend - dbegin + 1, 1);
      memcpy(actualVer, dbegin, dend - dbegin);
      fix_parser_set_error(
            parser,
            FIX_ERROR_WRONG_PROTOCOL_VER,
            "Wrong protocol. Expected '%s', actual '%s'.", parser->protocol->transportVersion, actualVer);
      free(actualVer);
      return NULL;
   }
   tag = parse_field(parser, dend + 1, len - (dend - data - 1), delimiter, &dbegin, &dend);
   if (tag == FIX_FAILED)
   {
      fix_parser_set_error(parser,FIX_ERROR_PARSE_MSG, "Unable to parse BodyLength field.");
      return NULL;
   }
   if (tag != FIXFieldTag_BodyLength)
   {
      fix_parser_set_error(parser,FIX_ERROR_WRONG_FIELD, "Second field is '%d', but must be BodyLength.", tag);
      return NULL;
   }
   int64_t bodyLen;
   if (fix_utils_atoi64(dbegin, dend - dbegin, 0, &bodyLen) == FIX_FAILED)
   {
      fix_parser_set_error(parser, FIX_ERROR_PARSE_MSG, "BodyLength value not a tagber.");
      return NULL;
   }
   if (bodyLen + CRC_FIELD_LEN > len - (dend - data))
   {
      return NULL;
   }
   char const* crcbeg = NULL;
   tag = parse_field(parser, dend + bodyLen + 1, CRC_FIELD_LEN, delimiter, &crcbeg, stop);
   if (tag == FIX_FAILED)
   {
      fix_parser_set_error(parser,FIX_ERROR_PARSE_MSG, "Unable to parse CrcSum field.");
      return NULL;
   }
   if (tag != FIXFieldTag_CheckSum)
   {
      fix_parser_set_error(parser,FIX_ERROR_WRONG_FIELD, "Field is '%d', but must be CrcSum.", tag);
      return NULL;
   }
   if (parser->flags & PARSER_FLAG_CHECK_CRC)
   {
      int64_t check_sum = 0;
      if (fix_utils_atoi64(crcbeg, *stop - crcbeg, 0, &check_sum) == FIX_FAILED)
      {
         fix_parser_set_error(parser, FIX_ERROR_PARSE_MSG, "CheckSum value not a tagber.");
         return NULL;
      }
      int64_t crc = 0;
      for(char const* it = data; it <= dend + bodyLen; ++it)
      {
         crc += *it;
      }
      crc %= 256;
      if (crc != check_sum)
      {
         fix_parser_set_error(
               parser,
               FIX_ERROR_INTEGRITY_CHECK, "CheckSum check failed. Expected '%d', actual '%d'.", check_sum, crc);
         return NULL;
      }
   }
   tag = parse_field(parser, dend + 1, len - (dend - data - 1), delimiter, &dbegin, &dend);
   if (tag == FIX_FAILED)
   {
      fix_parser_set_error(parser,FIX_ERROR_PARSE_MSG, "Unable to parse MsgType field.");
      return NULL;
   }
   if (tag != FIXFieldTag_MsgType)
   {
      fix_parser_set_error(parser,FIX_ERROR_WRONG_FIELD, "Field is '%d', but must be MsgType.", tag);
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
   while((len -= (dend - data - 1)) > 0)
   {
      tag = parse_field(parser, dend + 1, len, delimiter, &dbegin, &dend);
      if (tag == FIX_FAILED)
      {
         fix_parser_set_error(parser,FIX_ERROR_PARSE_MSG, "Unable to parse MsgType field.");
         return NULL;
      }
      fix_field_set(msg, NULL, tag, dbegin, dend - dbegin);
   }
   return msg;
}