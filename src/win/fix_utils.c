/* @file   fix_utils_win.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 12/27/2012 01:05:17 PM
*/

#include "fix_utils.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_utils_make_path(char const* protocolFile, char const* transpFile, char* path, uint32_t buffLen)
{
   char drive[_MAX_DRIVE + 1] = {};
   char dir[_MAX_DIR + 1] = {};
   char fname[_MAX_FNAME + 1] = {};
   char ext[_MAX_EXT + 1] = {};
   if (_splitpath_s(transpFile, drive, sizeof(drive), dir, sizeof(dir), fname, sizeof(fname), ext, sizeof(ext)))
   {
      return FIX_FAILED;
   }
   if (!strcmp(dir, "")) // looking for transpFile in protocolFile directory
   {
      char pdrive[_MAX_DRIVE + 1] = {};
      char pdir[_MAX_DIR + 1] = {};
      if (_splitpath_s(protocolFile, pdrive, sizeof(pdrive), pdir, sizeof(pdir), NULL, 0, NULL, 0))
      {
         return FIX_FAILED;
      }
      if (!strcmp(pdir, ""))  // protocolFile is in working directory too
      {
         if (strcpy_s(path, buffLen, transpFile))
         {
            return FIX_FAILED;
         }
      }
      else
      {
         if ((strlen(pdrive) + strlen(pdir) + strlen(transpFile) + 1) > buffLen)
         {
            return FIX_FAILED;
         }
         strcpy(path, pdrive);
         strcpy(path + strlen(pdrive), pdir);
         strcpy(path + strlen(pdrive) + strlen(pdir), transpFile);
      }
   }
   else // don't do anything, just return transFile path
   {
      if (strcpy_s(path, buffLen, transpFile))
      {
         return FIX_FAILED;
      }
   }
   return FIX_SUCCESS;
}
