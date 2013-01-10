/**
 * @file   fix_error_priv.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 10:26:54 AM
 */

#include "fix_error_priv.h"

#include <stdarg.h>
#include <stdio.h>

static FIXError error;

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_error_set_va(FIXError* error, FIXErrCode code, char const* text, va_list ap)
{
   error->code = code;
   int32_t n = vsnprintf(error->text, ERROR_TXT_SIZE, text, ap);
   error->text[n] = 0;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_error_set(FIXError* error, FIXErrCode code, char const* text, ...)
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
FIXError* fix_error_static_get()
{
   return &error;
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_error_static_set_va(FIXErrCode code, char const* text, va_list ap)
{
   fix_error_set_va(&error, code, text, ap);
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_error_static_set(FIXErrCode code, char const* text, ...)
{
   va_list ap;
   va_start(ap, text);
   fix_error_static_set_va(code, text, ap);
   va_end(ap);
}

/*------------------------------------------------------------------------------------------------------------------------*/
void fix_error_static_reset()
{
   fix_error_reset(&error);
}
