#include "protocol.h"
#include "error.h"

#include <stdio.h>

int main()
{
   Protocol* p = protocol_init("fix44.xml");
   if (!p)
   {
      printf("%s.\n", get_fix_last_error()->text);
   }
   else
   {
      printf("PROTOCOL %d\n", p->version);
   }
   return 0;
}
