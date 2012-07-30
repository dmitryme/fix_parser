#include "fix_parser.h"
#include "fix_error.h"

#include <stdio.h>

int main()
{
   int res = fix_protocol_init("fix44.xml");
   if (res == FIX_FAILED)
   {
      printf("ERROR: %s\n", get_fix_last_error()->text);
   }
   return 0;
}