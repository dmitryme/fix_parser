/// @file   fix_parser.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/30/2012 10:26:54 AM

#include "fix_parser.h"
#include "fix_protocol_descr.h"
#include "fix_error.h"


int fix_protocol_init(char const* protFile)
{
   FIXProtocolDescr* p = fix_protocol_descr_init(protFile);
   if (!p)
   {
      return FIX_FAILED;
   }
}

int parse_fix(FIXMessage** msg, uint32_t flags, char const* data)
{ 
   return 0;
}