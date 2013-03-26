/**
 * @file   fix_error.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 10:26:54 AM
 */

#include "fix_utils.h"
#include "fix_error.h"
#include "fix_error_priv.h"
#include "fix_parser_priv.h"

#include <stdarg.h>
#include <stdio.h>

//------------------------------------------------------------------------------------------------------------------------//
FIX_PARSER_API void fix_error_free(FIXError* error)
{
   if (error)
   {
      free(error);
   }
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API FIXErrCode fix_error_get_code(FIXError* error)
{
   if (!error)
   {
      return FIX_ERROR_INVALID_ARGUMENT;
   }
   return error->code;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIX_PARSER_API char const* fix_error_get_text(FIXError* error)
{
   if (!error)
   {
      return NULL;
   }
   return error->text;
}
