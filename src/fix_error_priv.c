/**
 * @file   fix_error_priv.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 10:26:54 AM
 */

#include "fix_error_priv.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*------------------------------------------------------------------------------------------------------------------------*/
FIXError* fix_error_create_va(FIXErrCode code, char const* text, va_list ap)
{
   FIXError* error = (FIXError*)calloc(1, sizeof(FIXError));
   error->code = code;
   int32_t n = vsnprintf(error->text, ERROR_TXT_SIZE, text, ap);
   error->text[n] = 0;
   return error;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXError* fix_error_create(FIXErrCode code, char const* text, ...)
{
   va_list ap;
   va_start(ap, text);
   FIXError* error = fix_error_create_va(code, text, ap);
   va_end(ap);
   return error;
}
