/**
 * @file   fix_parser_priv.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 10:26:54 AM
 */

#include "fix_parser_priv.h"
#include "fix_utils.h"
#include "fix_msg.h"

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
      page = (FIXPage*)calloc(1, sizeof(FIXPage) + psize - 1);
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
      group = (FIXGroup*)calloc(1, sizeof(FIXGroup));
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
int32_t fix_parser_validate_attrs(FIXParserAttrs* attrs)
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
      fix_error_static_set(FIX_ERROR_INVALID_ARGUMENT, "ERROR: Parser attbutes are invalid: MaxPageSize < PageSize.");
      return FIX_FAILED;
   }
   if (attrs->maxPages > 0 && attrs->maxPages < attrs->numPages)
   {
      fix_error_static_set(FIX_ERROR_INVALID_ARGUMENT, "Parser attbutes are invalid: MaxPages < NumPages.");
      return FIX_FAILED;
   }
   if (attrs->maxGroups > 0 && attrs->maxGroups < attrs->numGroups)
   {
      fix_error_static_set(FIX_ERROR_INVALID_ARGUMENT, "Parser attbutes are invalid: MaxGroups < NumGroups.");
      return FIX_FAILED;
   }
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_parser_check_value(FIXFieldDescr* fdescr, char const* dbegin, char const* dend, char delimiter)
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
FIXErrCode fix_parser_get_value(FIXParser* parser, char const* dbegin, uint32_t len, char delimiter, char const** dend)
{
   *dend = dbegin;
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
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXTagNum fix_parser_parse_mandatory_field(
      FIXParser* parser, char const* data, uint32_t len, char delimiter, char const** dbegin, char const** dend)
{
   FIXTagNum tag = 0;
   int32_t res = fix_utils_atoi32(data, len, '=', &tag);
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
   return tag;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXTagNum fix_parser_parse_field(
      FIXParser* parser, FIXMsg* msg, FIXGroup* group, char const* data, uint32_t len, char delimiter, FIXFieldDescr** fdescr, char const** dbegin, char const** dend)
{
   FIXTagNum tag = 0;
   FIXErrCode res = fix_utils_atoi32(data, len, '=', &tag);
   if (res == FIX_FAILED)
   {
      fix_error_set(&parser->error, FIX_ERROR_INVALID_ARGUMENT, "Unable to extract field number.");
      return FIX_FAILED;
   }
   len -= res;
   *dbegin = data + res + 1;
   *fdescr = group ? fix_protocol_get_group_descr(&parser->error, group->parent_fdescr, tag)
      : fix_protocol_get_field_descr(&parser->error, msg->descr, tag);
   if (!(*fdescr))
   {
      if (parser->flags & PARSER_FLAG_CHECK_UNKNOWN_FIELDS)
      {
         fix_error_set(&parser->error, FIX_ERROR_UNKNOWN_FIELD, "Field '%d' not found in description.", tag);
         return FIX_FAILED;
      }
      else // just skip field value
      {
         if (fix_parser_get_value(parser, *dbegin, len, delimiter, dend) == FIX_FAILED)
         {
            return FIX_FAILED;
         }
      }
   }
   if ((*fdescr)->type->valueType == FIXFieldValueType_Data) // extract value by DataLength field
   {
      int32_t dataLength;
      if (FIX_FAILED == fix_msg_get_int32(msg, group, (*fdescr)->dataLenField->type->tag, &dataLength))
      {
         fix_error_set(&parser->error, FIX_ERROR_UNKNOWN_FIELD, "Unable to get length field '%d'.", tag);
         return FIX_FAILED;
      }
      *dend = *dbegin + dataLength;
   }
   else // extract value till delimiter
   {
      if (fix_parser_get_value(parser, *dbegin, len, delimiter, dend) == FIX_FAILED)
      {
         return FIX_FAILED;
      }
   }
   return tag;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_parser_parse_group(
      FIXParser* parser, FIXMsg* msg, FIXGroup* parentGroup, FIXTagNum groupTag, int64_t numGroups, char const* data, uint32_t len, char delimiter, char const** stop)
{
   char const* dbegin = data;
   char const* dend = NULL;
   for(int64_t i = 0; i < numGroups; ++i)
   {
      FIXGroup* group = fix_msg_add_group(msg, parentGroup, groupTag);
      if (!group)
      {
         return FIX_FAILED;
      }
      FIXFieldDescr* fdescr = NULL;
      FIXTagNum tag = fix_parser_parse_field(parser, msg, group, dend + 1, data + len - dend, delimiter, &fdescr, &dbegin, stop);
      if (tag == FIX_FAILED)
      {
         return FIX_FAILED;
      }
      if (fdescr) // if !fdescr, ignore this field
      {
         if (parser->flags & PARSER_FLAG_CHECK_VALUE)
         {
            if (fix_parser_check_value(fdescr, dbegin, dend, delimiter) == FIX_FAILED)
            {
               return FIX_FAILED;
            }
         }
         if (fdescr->category == FIXFieldCategory_Value)
         {
            if (!fix_field_set(msg, group, fdescr, (unsigned char*)dbegin, dend - dbegin))
            {
               return FIX_FAILED;
            }
         }
         else if (fdescr->category == FIXFieldCategory_Group)
         {
            int32_t numGroups = 0;
            FIXErrCode res = fix_utils_atoi32(dbegin, dend - dbegin, delimiter, &numGroups);
            if (res == FIX_FAILED)
            {
               fix_error_set(&parser->error, FIX_ERROR_INVALID_ARGUMENT, "Unable to get group tag %d value.", tag);
               return FIX_FAILED;
            }
            res = fix_parser_parse_group(parser, msg, group, tag, numGroups, dend + 1, data + len - dend, delimiter, &dend);
            if (res == FIX_FAILED)
            {
               return FIX_FAILED;
            }
         }
      }
   }
   *stop = dend;
   return FIX_SUCCESS;
}
