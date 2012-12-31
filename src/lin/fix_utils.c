/* @file   fix_utils_lin.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 12/28/2012 04:58:17 PM
*/

#include "fix_utils.h"
#include "fix_types.h"
#include <libgen.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_utils_make_path(char const* protocolFile, char const* transpFile, char* path, uint32_t buffLen)
{
   FIXErrCode ret = FIX_SUCCESS;
   char* transpFileDup = strdup(transpFile);
   char* protocolFileDup = strdup(protocolFile);
   if (!strcmp(basename(transpFileDup), transpFile) &&
       strcmp(basename(protocolFileDup), protocolFile)) // make sure no dir(even ./) set for transpFile and protocolFile has a dir
   {
      if (snprintf(path, buffLen, "%s/%s", dirname(protocolFileDup), basename(transpFileDup)) >= buffLen)
      {
         ret = FIX_FAILED;
      }
   }
   else
   {
      if (snprintf(path, buffLen, "%s", transpFile) >= buffLen)
      {
         ret = FIX_FAILED;
      }
   }
   free(transpFileDup);
   free(protocolFileDup);
   return ret;
}
