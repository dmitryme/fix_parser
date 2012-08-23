/* @file   fix_parser.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:26:54 AM
*/

#include "fix_parser.h"
#include "fix_parser_priv.h"
#include "fix_protocol_descr.h"
#include "fix_message.h"
#include "fix_page.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/*------------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC                                                                                                                 */
/*------------------------------------------------------------------------------------------------------------------------*/

FIXParser* new_fix_parser(
      uint32_t pageSize, uint32_t maxPageSize, uint32_t numPages, uint32_t maxPages,
      uint32_t numGroups, uint32_t maxGroups, FIXParserFlags flags)
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
void free_fix_parser(FIXParser* parser)
{
   if (parser)
   {
      for(int i = 0; i < FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT; ++i)
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
int get_fix_error_code(FIXParser* parser)
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
int fix_protocol_init(FIXParser* parser, char const* protFile)
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
FIXPage* fix_parser_get_page(FIXParser* parser, uint32_t pageSize)
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
FIXGroup* fix_parser_get_group(FIXParser* parser)
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
void fix_parser_set_va_error(FIXParser* parser, int code, char const* text, va_list ap)
{
   parser->err_code = code;
   int n = vsnprintf(parser->err_text, ERROR_TXT_SIZE, text, ap);
   parser->err_text[n] = 0;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_parser_set_error(FIXParser* parser, int code, char const* text, ...)
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

//------------------------------------------------------------------------------------------------------------------------//
/*typedef enum ParserStateEnum_*/
/*{*/
/*   ParserState_Tag,*/
/*   ParserState_Value*/
/*} ParserStateEnum;*/

int parse_fix(FIXParser* parser, FIXMessage** msg, char const* data, uint32_t len)
{
   /*if (!data)*/
   /*{*/
   /*   fix_parser_set_error(FIX_ERROR_INVALID_ARGUMENT, "Data is empty");*/
   /*   return FIX_FAILED;*/
   /*}*/
   /*if (!msg)*/
   /*{*/
   /*   fix_parser_set_error(FIX_ERROR_INVALID_ARGUMENT, "Message storage is empty");*/
   /*   return FIX_FAILED;*/
   /*}*/
   /*uint32_t idx = 0;*/
   /*char tag[32] = {0};*/
   /*uint32_t tag_idx = 0;*/
   /*uint32_t value_size = 128;*/
   /*char* value = calloc(value_size, sizeof(char));*/
   /*uint32_t value_idx = 0;*/
   /*ParserStateEnum state = ParserState_Tag;*/
   /*FIXProtocolDescr* prot = NULL;*/
   /*while(idx < len)*/
   /*{*/
   /*   char ch = data[idx];*/
   /*   if (state == ParserState_Tag)*/
   /*   {*/
   /*      if (ch != '=')*/
   /*      {*/
   /*         if (tag_idx == sizeof(tag))*/
   /*         {*/
   /*            tag[sizeof(tag) - 1] = 0;*/
   /*            fix_parser_set_error(FIX_ERROR_INVALID_ARGUMENT, "Tag '%d' value is too long", tag);*/
   /*            return FIX_FAILED;*/
   /*         }*/
   /*         tag[tag_idx++] = ch;*/
   /*      }*/
   /*      else*/
   /*      {*/
   /*         state = ParserState_Value;*/
   /*      }*/
   /*   }*/
   /*   else if (state == ParserState_Value)*/
   /*   {*/
   /*      if (ch != SOH)*/
   /*      {*/
   /*         if (value_idx == value_size)*/
   /*         {*/
   /*            value = realloc(value, value_size *= 2);*/
   /*         }*/
   /*         else*/
   /*         {*/
   /*            value[value_idx] = ch;*/
   /*         }*/
   /*      }*/
   /*      else*/
   /*      {*/
   /*         if (strcmp(tag, "8")) // BeginString*/
   /*         {*/
   /*            FIXProtocolVerEnum ver = str2FIXProtocolVerEnum(value);*/
   /*            prot = get_fix_protocol_descr(ver);*/
   /*            if (!prot)*/
   /*            {*/
   /*               return FIX_FAILED;*/
   /*            }*/
   /*            state = ParserState_Tag;*/
   /*         }*/
   /*         else if (!strcmp(tag, "9")) // BodyLength*/
   /*         {*/
   /*            state = ParserState_Tag;*/
   /*         }*/
   /*         else if (!strcmp(tag, "35"))*/
   /*         {*/
   /*            *msg = create_fix_message(prot, value);*/
   /*            state = ParserState_Tag;*/
   /*         }*/
   /*         else*/
   /*         {*/
   /*            get_fix_tag_*/
   /*            set_fix_tag(*msg, tag, value);*/
   /*         }*/
   /*      }*/
   /*   }*/
   /*}*/
   return 0;
}