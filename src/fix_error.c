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
FIXErrCode fix_parser_get_error_code(FIXParser* parser)
{
   return parser->error.code;
}

/*------------------------------------------------------------------------------------------------------------------------*/
char const* fix_parser_get_error_text(FIXParser* parser)
{
   return parser->error.text;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_error_get_code()
{
   return error.code;
}

/*------------------------------------------------------------------------------------------------------------------------*/
char const* fix_error_get_text()
{
   return error.text;
}
