/**
 * @file   fix_field.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/25/2012 03:35:31 PM
 */

#include "fix_field.h"
#include "fix_parser.h"
#include "fix_parser_priv.h"
#include "fix_msg_priv.h"
#include "fix_utils.h"

#include <stdlib.h>
#include <string.h>

static FIXField* fix_field_free(FIXMsg* msg, FIXField* field);
static void fix_group_free(FIXMsg* msg, FIXGroup* group);

/*-----------------------------------------------------------------------------------------------------------------------*/
/* PUBLICS                                                                                                               */
/*-----------------------------------------------------------------------------------------------------------------------*/
FIXField* fix_field_set(FIXMsg* msg, FIXGroup* grp, FIXFieldDescr const* descr, unsigned char const* data, uint32_t len)
{
   FIXField* field = fix_field_get(msg, grp, descr->type->tag);
   if (!field && fix_parser_get_error_code(msg->parser))
   {
      return NULL;
   }
   if (field && field->descr->category == FIXFieldCategory_Group)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "FIXField has wrong type");
      return NULL;
   }
   int32_t idx = descr->type->tag % GROUP_SIZE;
   if (!field)
   {
      field = (FIXField*)fix_msg_alloc(msg, sizeof(FIXField));
      if (!field)
      {
         return NULL;
      }
      field->descr = descr;
      FIXGroup* group = (grp ? grp : msg->fields);
      field->next = group->fields[idx];
      group->fields[idx] = field;
      field->size = len;
      field->data = (char*)fix_msg_alloc(msg, len);
      field->body_len = 0;
   }
   else
   {
      field->size = len;
      field->data = (char*)fix_msg_realloc(msg, field->data, len);
      msg->body_len -= field->body_len;
   }
   if (!field->data)
   {
      return NULL;
   }
   if (LIKE(field->descr->type->tag != FIXFieldTag_BeginString &&
            field->descr->type->tag != FIXFieldTag_BodyLength &&
            field->descr->type->tag != FIXFieldTag_CheckSum))
   {
      field->body_len = fix_utils_numdigits(descr->type->tag) + 1 + len + 1;
   }
   memcpy(field->data, data, len);
   msg->body_len += field->body_len;
   return field;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXField* fix_field_get(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag)
{
   uint32_t const idx = tag % GROUP_SIZE;
   FIXField* it = (grp ? grp : msg->fields)->fields[idx];
   while(it)
   {
      if (it->descr->type->tag == tag)
      {
         return it;
      }
      it = it->next;
   }
   return it;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_field_del(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag)
{
   uint32_t const idx = tag % GROUP_SIZE;
   FIXGroup* group = grp ? grp : msg->fields;
   FIXField* field = group->fields[idx];
   FIXField* prev = field;
   while(field)
   {
      if (field->descr->type->tag == tag)
      {
         if (prev == field)
         {
            group->fields[idx] = fix_field_free(msg, field);
         }
         else
         {
            prev->next = fix_field_free(msg, field);
         }
         return FIX_SUCCESS;
      }
      prev = field;
      field = field->next;
   }
   fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_NOT_FOUND, "FIXField not found");
   return FIX_FAILED;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_group_add(FIXMsg* msg, FIXGroup* grp, FIXFieldDescr const* descr, FIXField** fld)
{
   FIXField* field = fix_field_get(msg, grp, descr->type->tag);
   FIXGroup* group = grp ? grp : msg->fields;
   if (field && field->descr->category != FIXFieldCategory_Group)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "FIXField has wrong type");
      return NULL;
   }
   if (!field)
   {
      uint32_t const idx = descr->type->tag % GROUP_SIZE;
      field = (FIXField*)fix_msg_alloc(msg, sizeof(FIXField));
      field->descr = descr;
      field->next = group->fields[idx];
      group->fields[idx] = field;
      field->data = (char*)fix_msg_alloc(msg, sizeof(FIXGroups));
      if (!field->data)
      {
         return NULL;
      }
      FIXGroups* grps = (FIXGroups*)field->data;
      field->size = 1;
      field->body_len = 0;
      grps->group[0] = fix_msg_alloc_group(msg);
      if (!grps->group[0])
      {
         return NULL;
      }
   }
   else
   {
      FIXGroups* grps = (FIXGroups*)field->data;
      FIXGroups* new_grps = (FIXGroups*)fix_msg_realloc(msg, field->data, sizeof(FIXGroups) + sizeof(FIXGroup*) * (field->size + 1));
      memcpy(new_grps->group, grps->group, sizeof(FIXGroup*) * (field->size));
      new_grps->group[field->size] = fix_msg_alloc_group(msg);
      if (!new_grps->group[field->size])
      {
         return NULL;
      }
      ++field->size;
      field->data = (char*)new_grps;
      msg->body_len -= field->body_len;
   }
   if (LIKE(field->descr->type->tag != FIXFieldTag_BeginString &&
            field->descr->type->tag != FIXFieldTag_BodyLength &&
            field->descr->type->tag != FIXFieldTag_CheckSum))
   {
      field->body_len = fix_utils_numdigits(descr->type->tag) + 1 + fix_utils_numdigits(field->size) + 1;
   }
   msg->body_len += field->body_len;
   FIXGroups* grps = (FIXGroups*)field->data;
   FIXGroup* new_grp = grps->group[field->size - 1];
   *fld = field;
   return new_grp;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXGroup* fix_group_get(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, uint32_t grpIdx)
{
   int32_t const idx = tag % GROUP_SIZE;
   FIXGroup* group = (grp ? grp : msg->fields);
   FIXField* it = group->fields[idx];
   while(it)
   {
      if (it->descr->type->tag == tag)
      {
         if (it->descr->category != FIXFieldCategory_Group)
         {
            fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "FIXField has wrong type");
            return NULL;
         }
         FIXGroups* grps = (FIXGroups*)it->data;
         if (grpIdx >= it->size)
         {
            fix_error_set(&msg->parser->error, FIX_ERROR_GROUP_WRONG_INDEX, "Wrong index");
            return NULL;
         }
         return grps->group[grpIdx];
      }
      else
      {
         it = it->next;
      }
   }
   return NULL;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXErrCode fix_group_del(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, uint32_t grpIdx)
{
   FIXField* field = fix_field_get(msg, grp, tag);
   if (!field)
   {
      return FIX_FAILED;
   }
   if (field->descr->category != FIXFieldCategory_Group)
   {
      fix_error_set(&msg->parser->error, FIX_ERROR_FIELD_HAS_WRONG_TYPE, "FIXField has wrong type");
      return FIX_FAILED;
   }
   FIXGroups* grps = (FIXGroups*)field->data;
   fix_group_free(msg, grps->group[grpIdx]);
   field->size -= 1;
   if (field->size == grpIdx)
   {
      grps->group[field->size] = NULL;
   }
   else
   {
      memcpy((char*)grps->group + sizeof(FIXGroup*) * grpIdx, (char*)grps->group + sizeof(FIXGroup*) * (grpIdx + 1),
         sizeof(FIXGroup*) * (field->size - grpIdx));
      grps->group[field->size] = NULL;
   }
   if (!field->size) // all groups has been deleted, so tag will be deleted either
   {
      return fix_field_del(msg, grp, tag);
   }
   else
   {
      msg->body_len -= field->body_len;
      field->body_len = fix_utils_numdigits(tag) + 1 + fix_utils_numdigits(field->size) + 1;
      msg->body_len += field->body_len;
   }
   return FIX_SUCCESS;
}

/*------------------------------------------------------------------------------------------------------------------------*/
/* PRIVATES                                                                                                               */
/*------------------------------------------------------------------------------------------------------------------------*/
static FIXField* fix_field_free(FIXMsg* msg, FIXField* field)
{
   if (field->descr->category == FIXFieldCategory_Group)
   {
      FIXGroups* grps = (FIXGroups*)field->data;
      for(uint32_t i = 0; i < field->size; ++i)
      {
         fix_group_free(msg, grps->group[i]);
      }
   }
   msg->body_len -= field->body_len;
   return field->next;
}

/*------------------------------------------------------------------------------------------------------------------------*/
static void fix_group_free(FIXMsg* msg, FIXGroup* group)
{
   for(int32_t i = 0; i < GROUP_SIZE; ++i)
   {
      FIXField* tag = group->fields[i];
      while(tag)
      {
         tag = fix_field_free(msg, tag);
      }
   }
   fix_msg_free_group(msg, group);
}
