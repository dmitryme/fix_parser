/* @file   fix_error.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:26:54 AM
*/

#include "fix_error.h"
#include "fix_error_priv.h"
#include "fix_parser_priv.h"

#include <stdarg.h>
#include <stdio.h>

static FIXError error;

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t get_fix_parser_error_code(FIXParser* parser)
{
   return parser->error.code;
}

/*------------------------------------------------------------------------------------------------------------------------*/
char const* get_fix_parser_error_text(FIXParser* parser)
{
   return parser->error.text;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t get_fix_error_code()
{
   return error.code;
}

/*------------------------------------------------------------------------------------------------------------------------*/
char const* get_fix_error_text()
{
   return error.text;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_error_set_va(FIXError* error, int32_t code, char const* text, va_list ap)
{
   error->code = code;
   int32_t n = vsnprintf(error->text, ERROR_TXT_SIZE, text, ap);
   error->text[n] = 0;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_error_set(FIXError* error, int32_t code, char const* text, ...)
{
   va_list ap;
   va_start(ap, text);
   fix_error_set_va(error, code, text, ap);
   va_end(ap);
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_error_reset(FIXError* error)
{
   error->code = 0;
   error->text[0] = 0;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXError* fix_static_error_get()
{
   return &error;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_static_error_set_va(int32_t code, char const* text, va_list ap)
{
   fix_error_set_va(&error, code, text, ap);
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_static_error_set(int32_t code, char const* text, ...)
{
   va_list ap;
   va_start(ap, text);
   fix_static_error_set_va(code, text, ap);
   va_end(ap);
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_static_error_reset()
{
   fix_error_reset(&error);
}

