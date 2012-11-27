/* @file   fix_parser_priv.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 10:26:54 AM
*/

#include "fix_parser_priv.h"
#include "fix_utils.h"

#include <string.h>

/*------------------------------------------------------------------------------------------------------------------------*/
FIXPage* fix_parser_alloc_page(FIXParser* parser, uint32_t pageSize)
{
   if (parser->attrs.maxPages > 0 && parser->attrs.maxPages == parser->used_pages)
   {
      fix_error_set(&parser->error,
         FIX_ERROR_NO_MORE_PAGES, "No more pages available. MaxPages = %d, UsedPages = %d", parser->attrs.maxPages, parser->used_pages);
      return NULL;
   }
   FIXPage* page = NULL;
   if (parser->page == NULL) // no more free pages
   {
      uint32_t psize = (parser->attrs.pageSize > pageSize ? parser->attrs.pageSize : pageSize);
      if (parser->attrs.maxPageSize > 0 && psize > parser->attrs.maxPageSize)
      {
         fix_error_set(
               &parser->error,
               FIX_ERROR_TOO_BIG_PAGE, "Requested new page is too big. MaxPageSize = %d, RequestedPageSize = %d",
               parser->attrs.maxPageSize, psize);
         return NULL;
      }
      page = calloc(1, sizeof(FIXPage) + psize - 1);
      page->size = psize;
   }
   else
   {
      page = parser->page;
      parser->page = page->next;
      page->next = NULL; // detach from pool of free pages
   }
   ++parser->used_pages;
   return page;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXPage* fix_parser_free_page(FIXParser* parser, FIXPage* page)
{
   FIXPage* next = page->next;
   page->offset = 0;
   page->next = parser->page;
   parser->page = page;
   --parser->used_pages;
   return next;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_parser_alloc_group(FIXParser* parser)
{
   if (parser->attrs.maxGroups > 0 && parser->attrs.maxGroups == parser->used_groups)
   {
      fix_error_set(&parser->error,
         FIX_ERROR_NO_MORE_GROUPS,
         "No more groups available. MaxGroups = %d, UsedGroups = %d", parser->attrs.maxGroups, parser->used_groups);
      return NULL;
   }
   FIXGroup* group = NULL;
   if (parser->group == NULL) // no more free group
   {
      group = calloc(1, sizeof(FIXGroup));
   }
   else
   {
      group = parser->group;
      parser->group = group->next;
      group->next = NULL; // detach from pool
   }
   ++parser->used_groups;
   return group;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_parser_free_group(FIXParser* parser, FIXGroup* group)
{
   FIXGroup* next = group->next;
   memset(group, 0, sizeof(FIXGroup));
   group->next = parser->group;
   parser->group = group;
   --parser->used_groups;
   return next;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int fix_parser_validate_attrs(FIXParserAttrs* attrs)
{
   if (!attrs->pageSize)
   {
      attrs->pageSize = 4096;
   }
   if (!attrs->numPages)
   {
      attrs->numPages = 1000;
   }
   if (!attrs->numGroups)
   {
      attrs->numGroups = 1000;
   }
   if (attrs->maxPageSize > 0 && attrs->maxPageSize < attrs->pageSize)
   {
      fix_static_error_set(FIX_ERROR_INVALID_ARGUMENT, "ERROR: Parser attbutes are invalid: MaxPageSize < PageSize.");
      return 0;
   }
   if (attrs->maxPages > 0 && attrs->maxPages < attrs->numPages)
   {
      fix_static_error_set(FIX_ERROR_INVALID_ARGUMENT, "Parser attbutes are invalid: MaxPages < NumPages.");
      return 0;
   }
   if (attrs->maxGroups > 0 && attrs->maxGroups < attrs->numGroups)
   {
      fix_static_error_set(FIX_ERROR_INVALID_ARGUMENT, "Parser attbutes are invalid: MaxGroups < NumGroups.");
      return 0;
   }
   return 1;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int fix_parser_check_value(FIXFieldDescr* fdescr, char const* dbegin, char const* dend, char delimiter)
{
   if (IS_INT_TYPE(fdescr->type->valueType))
   {
      int64_t res = 0;
      return fix_utils_atoi64(dbegin, dend - dbegin, delimiter, &res);
   }
   else if (IS_FLOAT_TYPE(fdescr->type->valueType))
   {
      double res = 0.0;
      return fix_utils_atod(dbegin, dend - dbegin, delimiter, &res);
   }
   else if (IS_CHAR_TYPE(fdescr->type->valueType))
   {
      return (dend - dbegin == 1) ? FIX_SUCCESS : FIX_FAILED;
   }
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int64_t fix_parser_parse_field(FIXParser* parser, char const* data, uint32_t len, char delimiter, char const** dbegin, char const** dend)
{
   int64_t num = 0;
   int32_t res = fix_utils_atoi64(data, len, '=', &num);
   if (res == FIX_FAILED)
   {
      fix_error_set(&parser->error, FIX_ERROR_INVALID_ARGUMENT, "Unable to extract field number.");
      return FIX_FAILED;
   }
   len -= res;
   *dend = *dbegin = data + res + 1;
   for(;len > 0; --len)
   {
      if (**dend == delimiter)
      {
         break;
      }
      else
      {
         ++(*dend);
      }
   }
   if (!len)
   {
      fix_error_set(&parser->error, FIX_ERROR_INVALID_ARGUMENT, "Field value must be terminated with '%c' delimiter.", delimiter);
      return FIX_FAILED;
   }
   return num;
}

/*------------------------------------------------------------------------------------------------------------------------*/
int32_t fix_parser_parse_group(FIXMsg* msg, int64_t numGroups, char const* data, uint32_t len, char delimiter, char const** stop)
{
   return FIX_SUCCESS;
}
