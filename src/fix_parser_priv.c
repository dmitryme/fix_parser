/**
 * @file   fix_parser_priv.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 10:26:54 AM
 */

#include "fix_parser_priv.h"
#include "fix_utils.h"
#include "fix_msg.h"
#include "fix_error_priv.h"

#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------------------------------------------------------*/
FIXPage* fix_parser_alloc_page(FIXParser* parser, uint32_t pageSize, FIXError** error)
{
   if (parser->attrs.maxPages > 0 && parser->attrs.maxPages == parser->used_pages)
   {
      *error = fix_error_create(
         FIX_ERROR_NO_MORE_PAGES, "No more pages available. MaxPages = %d, UsedPages = %d", parser->attrs.maxPages, parser->used_pages);
      return NULL;
   }
   FIXPage* page = NULL;
   if (parser->page == NULL) // no more free pages
   {
      uint32_t psize = (parser->attrs.pageSize > pageSize ? parser->attrs.pageSize : pageSize);
      if (parser->attrs.maxPageSize > 0 && psize > parser->attrs.maxPageSize)
      {
         *error = fix_error_create(
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
FIXGroup* fix_parser_alloc_group(FIXParser* parser, FIXError** error)
{
   if (parser->attrs.maxGroups > 0 && parser->attrs.maxGroups == parser->used_groups)
   {
      *error = fix_error_create(FIX_ERROR_NO_MORE_GROUPS,
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
int32_t fix_parser_validate_attrs(FIXParserAttrs* attrs, FIXError** error)
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
      *error = fix_error_create(FIX_ERROR_INVALID_ARGUMENT, "ERROR: Parser attbutes are invalid: MaxPageSize < PageSize.");
      return FIX_FAILED;
   }
   if (attrs->maxPages > 0 && attrs->maxPages < attrs->numPages)
   {
      *error = fix_error_create(FIX_ERROR_INVALID_ARGUMENT, "Parser attbutes are invalid: MaxPages < NumPages.");
      return FIX_FAILED;
   }
   if (attrs->maxGroups > 0 && attrs->maxGroups < attrs->numGroups)
   {
      *error = fix_error_create(FIX_ERROR_INVALID_ARGUMENT, "Parser attbutes are invalid: MaxGroups < NumGroups.");
      return FIX_FAILED;
   }
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_parser_check_value(FIXFieldDescr const* fdescr, char const* dbegin, char const* dend, char delimiter,
      FIXError** error)
{
   if (!fix_protocol_check_field_value(fdescr, dbegin, dend - dbegin))
   {
      *error = fix_error_create(FIX_ERROR_WRONG_FIELD_VALUE, "Wrong field '%s' value.", fdescr->type->name);
      return FIX_FAILED;
   }
   if (IS_INT_TYPE(fdescr->type->valueType))
   {
      int64_t res = 0;
      int32_t cnt;
      if(FIX_FAILED == fix_utils_atoi64(dbegin, dend - dbegin, delimiter, &res, &cnt))
      {
         *error = fix_error_create(FIX_ERROR_WRONG_FIELD_VALUE, "Wrong field '%s' value.", fdescr->type->name);
         return FIX_FAILED;
      }
   }
   else if (IS_FLOAT_TYPE(fdescr->type->valueType))
   {
      double res = 0.0;
      int32_t cnt;
      if (!fix_utils_atod(dbegin, dend - dbegin, delimiter, &res, &cnt))
      {
         *error = fix_error_create(FIX_ERROR_WRONG_FIELD_VALUE, "Wrong field '%s' value.", fdescr->type->name);
         return FIX_FAILED;
      }
   }
   else if (IS_CHAR_TYPE(fdescr->type->valueType))
   {
      if (dend - dbegin != 1)
      {
         *error = fix_error_create(FIX_ERROR_WRONG_FIELD_VALUE, "Wrong field '%s' value.", fdescr->type->name);
         return FIX_FAILED;
      }
   }
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
static FIXErrCode fix_parser_parse_value(FIXMsg* msg, FIXGroup* group, FIXFieldDescr const* fdescr,
      char const* dbegin, uint32_t len, char delimiter, char const** dend, FIXError** error)
{
   printf("parse_value '%.*s'\n", len, dbegin);
   *dend = dbegin;
   if (msg && fdescr && fdescr->type->valueType == FIXFieldValueType_Data) // Data field
   {
      int32_t dataLength;
      int32_t err = fix_msg_get_int32(msg, group, fdescr->dataLenField->type->tag, &dataLength, error);
      if (err < 0)
      {
         *error = fix_error_create(err, "Unable to get length field '%d'.", fdescr->dataLenField->type->tag);
         return FIX_FAILED;
      }
      *dend = dbegin + dataLength;
   }
   else // get value till delimiter
   {
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
         *error = fix_error_create(FIX_ERROR_NO_MORE_DATA, "Field value must be terminated with '%c' delimiter.", delimiter);
         return FIX_FAILED;
      }
   }
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
static FIXTagNum fix_parser_parse_tag(
      FIXMsg* msg, FIXGroup* group, char const* data, uint32_t len, FIXTagNum* tag, FIXFieldDescr const** fdescr,
      char const** dbegin, FIXError** error)
{
   printf("parse_tag '%.*s'\n", len, data);
   int32_t cnt;
   FIXErrCode res = fix_utils_atoi32(data, len, '=', tag, &cnt);
   if (res < 0)
   {
      *error = fix_error_create(res, "Unable to extract field number.");
      return FIX_FAILED;
   }
   *dbegin = data + cnt + 1;
   if (fdescr)
   {
      *fdescr = group ? fix_protocol_get_group_descr(group->parent_fdescr, *tag)
         : fix_protocol_get_field_descr(msg->descr, *tag);
   }
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXTagNum fix_parser_parse_mandatory_field(
      char const* data, uint32_t len, char delimiter, char const** dbegin, char const** dend, FIXError** error)
{
   FIXTagNum tag = 0;
   if(FIX_FAILED == fix_parser_parse_tag(NULL, NULL, data, len, &tag, NULL, dbegin, error))
   {
      return FIX_FAILED;
   }
   if (FIX_FAILED == fix_parser_parse_value(NULL, NULL, NULL, *dbegin, len - (*dbegin - data) , delimiter, dend, error))
   {
      return FIX_FAILED;
   }
   return tag;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXTagNum fix_parser_parse_field(
      FIXParser* parser, FIXMsg* msg, FIXGroup* group, char const* data, uint32_t len, char delimiter, FIXFieldDescr const** fdescr,
      char const** dbegin, char const** dend, FIXError** error)
{
   FIXTagNum tag = 0;
   if (FIX_FAILED == fix_parser_parse_tag(msg, group, data, len, &tag, fdescr, dbegin, error))
   {
      return FIX_FAILED;
   }
   if (!(*fdescr))
   {
      if (parser->flags & PARSER_FLAG_CHECK_UNKNOWN_FIELDS)
      {
         *error = fix_error_create(FIX_ERROR_UNKNOWN_FIELD, "Field '%d' not found in description.", tag);
         return FIX_FAILED;
      }
      else // just skip field value
      {
         if (FIX_FAILED == fix_parser_parse_value(NULL, NULL, NULL, *dbegin, len - (*dbegin - data), delimiter, dend, error))
         {
            return FIX_FAILED;
         }
      }
   }
   if (FIX_FAILED == fix_parser_parse_value(msg, group, *fdescr, *dbegin, len - (*dbegin - data), delimiter, dend, error))
   {
      return FIX_FAILED;
   }
   return tag;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_parser_parse_group(
      FIXParser* parser, FIXMsg* msg, FIXGroup* parentGroup, FIXFieldDescr const* gdescr, int64_t numGroups, char const* data,
      char const* bodyEnd, char delimiter, char const** stop, FIXError** error)
{
   printf("parse_group '%.*s'\n", (int)(bodyEnd - *stop), *stop);
   FIXFieldDescr* first_req_field = &gdescr->group[0]; // first required field MUST be present in string
   FIXGroup* group = NULL;
   int32_t groupCount = 0;
   *stop = data;
   while(numGroups && bodyEnd != *stop) // if number of groups = 0, nothing to do
   {
      FIXTagNum tag = 0;
      char const* dbegin = NULL;
      FIXFieldDescr const* fdescr = NULL;
      if(FIX_FAILED == fix_parser_parse_tag(NULL, NULL, *stop + 1, bodyEnd - *stop, &tag, NULL, &dbegin, error))
      {
         return FIX_FAILED;
      }
      if (tag == first_req_field->type->tag) // start of new group
      {
         if (group && (parser->flags & PARSER_FLAG_CHECK_REQUIRED)) // previous group has already parsed, check it if needed
         {
            for(uint32_t i = 0; i < gdescr->group_count; ++i)
            {
               FIXFieldDescr* fdescr = &gdescr->group[i];
               if (fdescr->flags & FIELD_FLAG_REQUIRED)
               {
                  if (!fix_field_get(msg, group, fdescr->type->tag))
                  {
                     *error = fix_error_create(FIX_ERROR_FIELD_NOT_FOUND,
                           "Required field '%s' not found in group '%s'.",
                           fdescr->type->name, group->parent_fdescr->type->name);
                     return FIX_FAILED;
                  }
               }
            }
         }
         group = fix_msg_add_group(msg, parentGroup, gdescr->type->tag, error);
         if (!group)
         {
            return FIX_FAILED;
         }
         ++groupCount;
         fdescr = first_req_field;
      }
      else
      {
         fdescr = fix_protocol_get_group_descr(group->parent_fdescr, tag);
         if (!fdescr)
         {
            if (groupCount == numGroups) // looks like we finished
            {
               return FIX_SUCCESS;
            }
            else
            {
               *error = fix_error_create(
                     FIX_ERROR_UNKNOWN_FIELD, "Field '%d' not found in group '%s' description.", tag, gdescr->type->name);
               return FIX_FAILED;
            }
         }
      }
      if (FIX_FAILED == fix_parser_parse_value(msg, group, fdescr, dbegin, bodyEnd - dbegin + 1, delimiter, stop, error))
      {
         return FIX_FAILED;
      }
      if (parser->flags & PARSER_FLAG_CHECK_VALUE)
      {
         if (fix_parser_check_value(fdescr, dbegin, *stop, delimiter, error) == FIX_FAILED)
         {
            return FIX_FAILED;
         }
      }
      if (fdescr->category == FIXFieldCategory_Value)
      {
         if (!fix_field_set(msg, group, fdescr, (unsigned char*)dbegin, *stop - dbegin, error))
         {
            return FIX_FAILED;
         }
      }
      else if (fdescr->category == FIXFieldCategory_Group)
      {
         int32_t numGroups = 0;
         int32_t cnt;
         FIXErrCode err = fix_utils_atoi32(dbegin, *stop - dbegin, delimiter, &numGroups, &cnt);
         if (err < 0)
         {
            *error = fix_error_create(err, "Unable to get group tag %d value.", tag);
            return FIX_FAILED;
         }
         err = fix_parser_parse_group(parser, msg, group, fdescr, numGroups, *stop, bodyEnd, delimiter, stop, error);
         if (err == FIX_FAILED)
         {
            return FIX_FAILED;
         }
      }
   }
   return FIX_SUCCESS;
}
