/// @file   fix_parser.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/30/2012 10:26:54 AM

#include "fix_parser.h"
#include "fix_protocol_descr.h"
#include "fix_error.h"
#include "fix_message.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define SOH 0x01

extern void set_fix_error(int, char const*, ...);

int fix_protocol_init(char const* protFile)
{
   FIXProtocolDescr* p = fix_protocol_descr_init(protFile);
   if (!p)
   {
      return FIX_FAILED;
   }
}

typedef enum ParserStateEnum_
{
   ParserState_Tag,
   ParserState_Value
} ParserStateEnum;

int parse_fix(FIXMessage** msg, uint32_t flags, char const* data, uint32_t len)
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