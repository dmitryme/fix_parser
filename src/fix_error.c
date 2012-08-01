/// @file   fix_error.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/24/2012 06:16:10 PM

#include "fix_error.h"

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define ERROR_TXT_SIZE 1024

typedef struct FIXError_
{
   int code;
   char text[ERROR_TXT_SIZE];
} FIXError;

FIXError error;

int get_fix_error_code()
{
   return error.code;
}

char const* get_fix_error_text()
{
   return error.text;
}

void reset_fix_error()
{
   error.code = 0;
   error.text[0] = 0;
}

void set_fix_va_error(int code, char const* text, va_list ap)
{
   error.code = code;
   int n = vsnprintf(error.text, ERROR_TXT_SIZE, text, ap);
   error.text[n - 1] = 0;
}

void set_fix_error(int code, char const* text, ...)
{
   va_list ap;
   va_start(ap, text);
   set_fix_va_error(code, text, ap);
   va_end(ap);
}