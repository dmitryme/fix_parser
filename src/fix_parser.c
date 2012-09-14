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
      for(int32_t i = 0; i < FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT; ++i)
      {
         if (parser->protocols[i])
         {
            free_fix_protocol_descr(parser->protocols[i]);
         }
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
   FIXProtocolDescr* p = new_fix_protocol_descr(parser, protFile);
   if (!p)
   {
      return FIX_FAILED;
   }
   if (parser->protocols[p->version])
   {
      free_fix_protocol_descr(parser->protocols[p->version]);
   }
   parser->protocols[p->version] = p;
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
FIXProtocolDescr* fix_parser_get_pdescr(FIXParser* parser, FIXProtocolVerEnum ver)
{
   if (ver < 0 || ver >= FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT)
   {
      fix_parser_set_error(parser, FIX_ERROR_WRONG_PROTOCOL_VER, "Wrong FIX protocol version %d", ver);
      return NULL;
   }
   return parser->protocols[ver];
}

/*[>------------------------------------------------------------------------------------------------------------------------<]*/
/*int32_t parse_field(FIXParser* parser, char const* data, uint32_t len, uint32_t* num, char* value)*/
/*{*/
/*   *num = 0;*/
/*   int32_t res = fix_utils_atoi64(data, len, '=', &num);*/
/*   if (res == FIX_FAILED)*/
/*   {*/
/*      fix_parser_set_error(parser, FIX_ERROR_INVALID_ARGUMENT, "Unable to extract field number.");*/
/*      return FIX_FAILED;*/
/*   }*/
/*   data += res;*/
/*   int val_len = 0;*/
/*   for(;*data != delimiter; ++val_len, ++value, ++data)*/
/*   {*/
/*      *value = *data;*/
/*   }*/
/*   *value = 0;*/
/*   return val_len;*/
/*}*/

/*[>------------------------------------------------------------------------------------------------------------------------<]*/
/*int32_t parser_header(FIXParser* parser, char const* data, uint32_t len, FIXProtocolVerEnum& ver, char* msgType, size_t msgTypeLen, uint32_t* bodyLen)*/
/*{*/
/*   char value[len + 1];*/
/*   uint32_t num = 0;*/
/*   int32_t res = parse_field(parser, data, len, &num, value);*/
/*   if (res == FIX_FAILED)*/
/*   {*/
/*      // TODO:*/
/*      return FIX_FAILED;*/
/*   }*/
/*   if (num != FIXTagNum_BeginString)*/
/*   {*/
/*      //TODO:*/
/*      return FIX_FAILED;*/
/*   }*/
/*   FIXProtocolVerEnum mver = str2FIXProtocolVerEnum(value);*/
/*   res = parse_field(parser, data, len, &num, value);*/
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
/*   *msg = fix_msg_create(parser, ver, value);*/
/*   if (!*msg)*/
/*   {*/
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

/*[>------------------------------------------------------------------------------------------------------------------------<]*/
/*int32_t parse_fix(FIXParser* parser, char const* data, uint32_t len, FIXProtocolVerEnum ver, char delimiter, FIXMsg** msg)*/
/*{*/
/*   if (!parser || !data || !*msg)*/
/*   {*/
/*      return FIX_FAILED;*/
/*   }*/
/*   FIXProtocolDescr* pdescr = parser->protocols[ver];*/
/*   if (!pdescr)*/
/*   {*/
/*      fix_parser_set_error(parser, FIX_ERROR_WRONG_PROTOCOL_VER, "Wrong FIX protocol version %d", ver);*/
/*      return FIX_FAILED;*/
/*   }*/
/*   FIXProtocolVerEnum mver;*/
/*   char msgType[10];*/
/*   uint32_t bodyLen = 0;*/
/*   parser_header(data, len, &mver, msgType, sizeof(msgType), &bodyLen);*/
/*   if ((mver != FIXT11 && mver != ver) ||*/
/*         (mver == FIXT11 && ver != FIX50 && ver != FIX50SP1 && ver != FIX50SP2))*/
/*   {*/
/*      fix_parser_set_error(parser, FIX_ERROR_WRONG_PROTOCOL_VER, "Wrong protocol '%s' version", value);*/
/*      return FIX_FAILED;*/
/*   }*/
/*   FIXMsgDescr* mdescr = NULL;*/
/*      FIXFieldDescr* fdescr = fix_protocol_get_field_descr(parser, mdescr, num);*/
/*      if (!fdescr && (parser->flags & PARSER_FLAG_CHECK_EXISTING))*/
/*      {*/
/*         fix_parser_set_error(parser, FIX_ERROR_UNKNOWN_FIELD, "Field with tag '%d' not found in message '%s' description.", num, pdescr->name);*/
/*         return FIX_FAILED;*/
/*      }*/
/*      if (fdescr && fdescr->field_type->) // value*/
/*      {*/
/*         fix_tag_set(*msg, NULL, num, data, len);*/
/*      }*/
/*      else if (fdescr && fdescr->field_type) // group*/
/*      {*/
/*         fix_group_add(*msg, NULL, num, NULL);*/
/*      }  */
/*   [>} <]*/
/*   return 0;*/
/*}*/