/// @file   fix_parser.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/30/2012 10:26:54 AM

#include "fix_parser.h"
#include "fix_protocol_descr.h"
#include "fix_message.h"
#include "fix_page.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define SOH 0x01
#define ERROR_TXT_SIZE 1024

struct FIXParser_
{
   FIXProtocolDescr* protocols[FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT - 1];
   int err_code;
   char err_text[ERROR_TXT_SIZE];
   uint32_t flags;
   FIXPage* page;
   FIXPage* free_page;
   uint32_t num_pages;
   uint32_t page_size;
};

//------------------------------------------------------------------------------------------------------------------------//
FIXParser* new_fix_parser(uint32_t pageSize, uint32_t numPages, uint32_t flags)
{
   FIXParser* parser = calloc(1, sizeof(FIXParser));
   parser->flags = flags;
   parser->page_size = pageSize;
   for(uint32_t i = 0; i < numPages; ++i)
   {
      FIXPage* page = calloc(1, sizeof(FIXPage) + pageSize - 1);
      page->size = pageSize;
      page->next = parser->page;
      parser->page = page;
      parser->free_page = page;
   }
   parser->num_pages = numPages;
   return parser;
}

//------------------------------------------------------------------------------------------------------------------------//
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
      free(parser);
   }
}

//------------------------------------------------------------------------------------------------------------------------//
FIXPage* fix_parser_get_page(FIXParser* parser)
{
   FIXPage* page = NULL;
   if (parser->free_page == NULL) // no mode free pages
   {
      page = calloc(1, sizeof(FIXPage) + parser->page_size - 1);
      page->size = parser->page_size;
      parser->free_page = page;
      ++parser->num_pages;
   }
   else
   {
      page = parser->free_page;
      parser->free_page = page->next;
      page->next = NULL; // detach from pool
   }
   return page;
}

//------------------------------------------------------------------------------------------------------------------------//
void fix_parser_free_page(FIXParser* parser, FIXPage* page)
{
   page->offset = 0;
   page->next = parser->free_page;
   parser->free_page = page;
}

//------------------------------------------------------------------------------------------------------------------------//
int get_fix_error_code(FIXParser* parser)
{
   if (parser)
   {
      return parser->err_code;
   }
   return FIX_FAILED;
}

//------------------------------------------------------------------------------------------------------------------------//
char const* get_fix_error_text(FIXParser* parser)
{
   if (parser)
   {
      return parser->err_text;
   }
   return NULL;
}

//------------------------------------------------------------------------------------------------------------------------//
int get_fix_parser_flags(FIXParser* parser)
{
   if (parser)
   {
      return parser->flags;
   }
   return FIX_FAILED;
}

//------------------------------------------------------------------------------------------------------------------------//
void set_fix_va_error(FIXParser* parser, int code, char const* text, va_list ap)
{
   parser->err_code = code;
   int n = vsnprintf(parser->err_text, ERROR_TXT_SIZE, text, ap);
   parser->err_text[n - 1] = 0;
}

//------------------------------------------------------------------------------------------------------------------------//
void set_fix_error(FIXParser* parser, int code, char const* text, ...)
{
   va_list ap;
   va_start(ap, text);
   set_fix_va_error(parser, code, text, ap);
   va_end(ap);
}

//------------------------------------------------------------------------------------------------------------------------//
void reset_fix_error(FIXParser* parser)
{
   parser->err_code = 0;
   parser->err_text[0] = 0;
}

//------------------------------------------------------------------------------------------------------------------------//
int fix_protocol_init(FIXParser* parser, char const* protFile)
{
   if(!parser)
   {
      return FIX_FAILED;
   }
   FIXProtocolDescr* p = fix_protocol_descr_init(parser, protFile);
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

//------------------------------------------------------------------------------------------------------------------------//
FIXProtocolDescr* get_fix_protocol_descr(FIXParser* parser, FIXProtocolVerEnum ver)
{
   if (ver < 0 || ver >= FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT)
   {
      set_fix_error(parser, FIX_ERROR_WRONG_PROTOCOL_VER, "Wrong FIX protocol version %d", ver);
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
   /*   set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Data is empty");*/
   /*   return FIX_FAILED;*/
   /*}*/
   /*if (!msg)*/
   /*{*/
   /*   set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Message storage is empty");*/
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
   /*            set_fix_error(FIX_ERROR_INVALID_ARGUMENT, "Tag '%d' value is too long", tag);*/
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