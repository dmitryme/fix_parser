/// @file   fix_error.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/24/2012 06:16:10 PM

#include "fix_error.h"

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

FIXError* err = 0;

void free_fix_error(FIXError* err)
{
   if (err && err->code)
   {
      free(err->text);
      err->code = 0;
   }
}

void init_fix_error()
{
   if (!err)
   {
      err = calloc(1, sizeof(FIXError));
   }
}

FIXError* get_fix_last_error()
{
   init_fix_error();
   return err;
}

void reset_fix_error()
{
   free_fix_error(err);
}

void set_fix_va_error(int code, char const* text, va_list ap)
{
   init_fix_error();
   free_fix_error(err);
   err->code = code;

   int size = 100;     /* Guess we need no more than 100 bytes. */

   if ((err->text = malloc(size)) == NULL)
   {
      assert(!"Unable to allocate for error");
   }

   while (1)
   {
      va_list ap1;
      va_copy(ap1, ap);
      int n = vsnprintf(err->text, size, text, ap1);
      va_end(ap1);
      if (n > -1 && n < size)
      {
         return;
      }
      else if (n >= size)
      {
         size *= 2;
      }
      else if (n == -1)
      {
         assert(!"set_fix_error failed");
      }

      if ((err->text = realloc (err->text, size)) == NULL)
      {
         assert(!"Unable to realloc for error.");
      }
  }
}

void set_fix_error(int code, char const* text, ...)
{
   va_list ap;
   va_start(ap, text);
   set_fix_va_error(code, text, ap);
   va_end(ap);
}