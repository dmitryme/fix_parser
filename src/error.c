/// @file   error.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/24/2012 06:16:10 PM

#include "error.h"

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

FIXError* err = 0;

void free_error(FIXError* err)
{
   if (err && err->code)
   {
      free(err->text);
      err->code = 0;
   }
}

void init_error()
{
   if (!err)
   {
      err = calloc(1, sizeof(FIXError));
   }
}

FIXError* get_fix_last_error()
{
   init_error();
   return err;
}

void reset_fix_error()
{
   free_error(err);
}

void set_fix_error(int code, char const* text, ...)
{
   init_error();
   free_error(err);
   err->code = code;

   int size = 100;     /* Guess we need no more than 100 bytes. */
   va_list ap;

   if ((err->text = malloc(size)) == NULL)
   {
      assert(!"Unable to allocate for error");
   }

   while (1)
   {
      va_start(ap, text);
      int n = vsnprintf(err->text, size, text, ap);
      va_end(ap);

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