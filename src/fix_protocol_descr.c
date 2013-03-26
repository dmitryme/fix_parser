/**
 * @file   fix_protocol_descr.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/27/2012 06:14:53 PM
 */

#include "fix_protocol_descr.h"
#include "fix_utils.h"
#include "fix_types.h"
#include "fix_parser_priv.h"
#include "fix_descr_xsd.h"
#include "fix_error_priv.h"

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include  <libxml/tree.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>

/*-----------------------------------------------------------------------------------------------------------------------*/
/* PRIVATES                                                                                                              */
/*-----------------------------------------------------------------------------------------------------------------------*/
static void xmlErrorHandler(void* ctx, char const* msg, ...)
{
   va_list ap;
   va_start(ap, msg);
   FIXError** error = (FIXError**)ctx;
   *error = fix_error_create_va(FIX_ERROR_LIBXML, msg, ap);
   va_end(ap);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static int32_t initLibXml(FIXError** error)
{
   xmlSetGenericErrorFunc(error, xmlErrorHandler);
   return 0;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static FIXErrCode xml_validate(xmlDoc* doc, FIXError** error)
{
   xmlSchemaParserCtxtPtr pctx = xmlSchemaNewMemParserCtxt(fix_xsd, strlen(fix_xsd));
   xmlSchemaPtr schema = xmlSchemaParse(pctx);
   if (!schema)
   {
      return FIX_FAILED;
   }
   xmlSchemaValidCtxtPtr validCtx = xmlSchemaNewValidCtxt(schema);
   xmlSchemaSetValidErrors(validCtx, &xmlErrorHandler, &xmlErrorHandler, error);

   int32_t res = xmlSchemaValidateDoc(validCtx, doc);

   xmlSchemaFreeValidCtxt(validCtx);
   xmlSchemaFree(schema);
   xmlSchemaFreeParserCtxt(pctx);

   return res ? FIX_FAILED : FIX_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static char const* get_attr(xmlNode const* node, char const* attrName, char const* defVal)
{
   if (!node)
   {
      return NULL;
   }
   xmlAttr const* attr = node->properties;
   while(attr)
   {
      if (!strcmp((char const*)attr->name, attrName))
      {
         return (char const*)attr->children->content;
      }
      attr = attr->next;
   }
   return defVal;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static xmlNode* get_first(xmlNode const* node, char const* name)
{
   xmlNode* child = node->children;
   while(child)
   {
      if (child->type == XML_ELEMENT_NODE && !strcmp((char const*)child->name, name))
      {
         return child;
      }
      child = child->next;
   }
   return NULL;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static void free_field_descr(FIXFieldDescr* fd)
{
   for(uint32_t i = 0; i < fd->group_count; ++i)
   {
      free_field_descr(&fd->group[i]);
   }
   free(fd->group);
   free(fd->group_index);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static void free_field_type(FIXFieldType const* ft)
{
   if (ft->values)
   {
      for(int32_t i = 0; i < FIELD_VALUE_CNT; ++i)
      {
         FIXFieldValue* fval = ft->values[i];
         while(fval)
         {
            FIXFieldValue* next = fval->next;
            free((void*)fval->value);
            free((void*)fval);
            fval = next;
         }
      }
      free((void*)ft->values);
   }
   free(ft->name);
   free((void*)ft);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static void free_message(FIXMsgDescr const* msg)
{
   free(msg->name);
   free(msg->type);
   free(msg->field_index);
   for(uint32_t i = 0; i < msg->field_count; ++i)
   {
      free_field_descr(&msg->fields[i]);
   }
   free(msg->fields);
   free((void*)msg);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static FIXErrCode load_field_types(FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], xmlNode const* root, FIXError** error)
{
   xmlNode const* field = get_first(get_first(root, "fields"), "field");
   while(field)
   {
      if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "field"))
      {
         if (fix_protocol_get_field_type(ftypes, get_attr(field, "name", NULL)))
         {
            *error = fix_error_create(FIX_ERROR_FIELD_TYPE_EXISTS, "FIXFieldType '%s' already exists", (char const*)field->name);
            return FIX_FAILED;
         }
         FIXFieldType* fld = (FIXFieldType*)calloc(1, sizeof(FIXFieldType));
         fld->tag = atoi(get_attr(field, "number", NULL));
         fld->name = _strdup(get_attr(field, "name", NULL));
         fld->valueType = str2FIXFieldValueType(get_attr(field, "type", NULL));
         xmlNode const* value = get_first(field, "value");
         if (value)
         {
            fld->values = (FIXFieldValue**)calloc(FIELD_VALUE_CNT, sizeof(FIXFieldValue*));
            while(value)
            {
               if (value->type == XML_ELEMENT_NODE && !strcmp((char const*)value->name, "value"))
               {
                  FIXFieldValue* val = (FIXFieldValue*)calloc(1, sizeof(FIXFieldValue));
                  val->value = strdup(get_attr(value, "enum", NULL));
                  uint32_t idx = fix_utils_hash_string(val->value, strlen(val->value)) % FIELD_VALUE_CNT;
                  val->next = fld->values[idx];
                  fld->values[idx] = val;
               }
               value = value->next;
            }
         }
         uint32_t idx = fix_utils_hash_string(fld->name, strlen(fld->name)) % FIELD_TYPE_CNT;
         fld->next = (*ftypes)[idx];
         (*ftypes)[idx] = fld;
      }
      field = field->next;
   }
   return FIX_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static uint32_t count_msg_fields(xmlNode const* msg_node, xmlNode const* components)
{
   uint32_t count = 0;
   xmlNode const* field = msg_node->children;
   while(field)
   {
      if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "field"))
      {
         ++count;
      }
      else if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "component"))
      {
         char const* component_name = get_attr(field, "name", NULL);
         xmlNode* component = get_first(components, "component");
         while(component)
         {
            if (component->type == XML_ELEMENT_NODE)
            {
               char const* name = get_attr(component, "name", NULL);
               if (!strcmp(component_name, name))
               {
                  count += count_msg_fields(component, components);
                  break;
               }
            }
            component = component->next;
         }
      }
      else if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "group"))
      {
         ++count;
      }
      field = field->next;
   }
   return count;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static FIXErrCode load_fields(
      FIXFieldDescr* fields, uint32_t* count, xmlNode const* msg_node, xmlNode const* components,
      FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], FIXError** error)
{
   xmlNode const* field = msg_node->children;
   while(field)
   {
      if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "field"))
      {
         char const* name = get_attr(field, "name", NULL);
         char const* required = get_attr(field, "required", NULL);
         FIXFieldDescr* fld = &fields[(*count)++];
         fld->type = fix_protocol_get_field_type(ftypes, name);
         fld->category = FIXFieldCategory_Value;
         if (!fld->type)
         {
            *error = fix_error_create(FIX_ERROR_UNKNOWN_FIELD, "FIXFieldType '%s' is unknown", name);
            return FIX_FAILED;
         }
         if (!strcmp(required, "Y"))
         {
            fld->flags |= FIELD_FLAG_REQUIRED;
         }
         if (fld->type->valueType == FIXFieldValueType_Data)
         {
            if ((*count) < 2)
            {
               *error = fix_error_create(FIX_ERROR_WRONG_FIELD, "Previous field for field '%s' shall have Length type.", name);
               return FIX_FAILED;
            }
            // get previous field. It must be Length data type
            FIXFieldDescr* prevFld = &fields[(*count) - 2];
            if (prevFld->type->valueType != FIXFieldValueType_Length)
            {
               *error = fix_error_create(FIX_ERROR_WRONG_FIELD, "Previous field for field '%s' shall have Length type.", name);
               return FIX_FAILED;
            }
            fld->dataLenField = prevFld;
         }
      }
      else if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "component"))
      {
         char const* component_name = get_attr(field, "name", NULL);
         xmlNode* component = get_first(components, "component");
         while(component)
         {
            if (component->type == XML_ELEMENT_NODE)
            {
               char const* name = get_attr(component, "name", NULL);
               if (!strcmp(component_name, name))
               {
                  if (FIX_FAILED == load_fields(fields, count, component, components, ftypes, error))
                  {
                     return FIX_FAILED;
                  }
               }
            }
            component = component->next;
         }
      }
      else if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "group"))
      {
         char const* name = get_attr(field, "name", NULL);
         char const* required = get_attr(field, "required", NULL);
         FIXFieldDescr* fld = &fields[(*count)++];
         memset(fld, 0, sizeof(FIXFieldDescr));
         fld->type = fix_protocol_get_field_type(ftypes, name);
         fld->category = FIXFieldCategory_Group;
         if (!fld->type)
         {
            *error = fix_error_create(FIX_ERROR_UNKNOWN_FIELD, "FIXFieldType '%s' is unknown", name);
            return FIX_FAILED;
         }
         if (!strcmp(required, "Y"))
         {
            fld->flags |= FIELD_FLAG_REQUIRED;
         }
         fld->group_index = (FIXFieldDescr**)calloc(FIELD_DESCR_CNT, sizeof(FIXFieldDescr*));
         fld->group_count = count_msg_fields(field, components);
         fld->group = (FIXFieldDescr*)calloc(fld->group_count, sizeof(FIXFieldDescr));
         uint32_t count1 = 0;
         if (FIX_FAILED == load_fields(fld->group, &count1, field, components, ftypes, error))
         {
            return FIX_FAILED;
         }
      }
      field = field->next;
   }
   return FIX_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static void build_index(FIXFieldDescr* fields, uint32_t field_count, FIXFieldDescr** index)
{
   for(uint32_t i = 0; i < field_count; ++i)
   {
      FIXFieldDescr* fld = &fields[i];
      int32_t idx = fld->type->tag % FIELD_DESCR_CNT;
      fld->next = index[idx];
      index[idx] = fld;
      if (fld->group_count)
      {
         build_index(fld->group, fld->group_count, fld->group_index);
      }
   }
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static FIXMsgDescr* load_message(xmlNode const* msg_node, xmlNode const* root,
      FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], FIXError** error)
{
   FIXMsgDescr* msg = (FIXMsgDescr*)calloc(1, sizeof(FIXMsgDescr));
   msg->name = _strdup(get_attr(msg_node, "name", NULL));
   msg->type = _strdup(get_attr(msg_node, "type", NULL));
   msg->field_count = count_msg_fields(msg_node, get_first(root, "components"));
   msg->fields = (FIXFieldDescr*)calloc(msg->field_count, sizeof(FIXFieldDescr));
   uint32_t count = 0;
   if (FIX_FAILED == load_fields(msg->fields, &count, msg_node, get_first(root, "components"), ftypes, error))
   {
      return NULL;
   }
   assert(count == msg->field_count);
   msg->field_index = (FIXFieldDescr**)calloc(FIELD_DESCR_CNT, sizeof(FIXFieldDescr*));
   build_index(msg->fields, msg->field_count, msg->field_index);
   return msg;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static int32_t load_messages(FIXProtocolDescr* prot, FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], xmlNode const* root,
      FIXError** error)
{
   xmlNode* msg_node = get_first(get_first(root, "messages"), "message");
   while(msg_node)
   {
      if (msg_node->type == XML_ELEMENT_NODE && !strcmp((char const*)msg_node->name, "message"))
      {
         FIXMsgDescr* msg = load_message(msg_node, root, ftypes, error);
         if (!msg)
         {
            return FIX_FAILED;
         }
         int32_t idx = fix_utils_hash_string(msg->type, strlen(msg->type)) % MSG_CNT;
         msg->next = prot->messages[idx];
         prot->messages[idx] = msg;
      }
      msg_node = msg_node->next;
   }
   return FIX_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static int32_t load_transport_protocol(FIXProtocolDescr* prot, xmlNode* parentRoot, char const* parentFile, FIXError** error)
{
   int32_t res = FIX_SUCCESS;
   xmlDoc* doc = NULL;
   char const* transpFile = get_attr(parentRoot, "transport", parentFile);
   if(!strcmp(transpFile, parentFile)) // transport is the same as protocol
   {
      prot->transportVersion = _strdup(prot->version);
      goto ok;
   }
   char path[PATH_MAX] = {};
   if (FIX_FAILED == fix_utils_make_path(parentFile, transpFile, path, PATH_MAX))
   {
      goto err;
   }
   doc = xmlParseFile(path);
   if (!doc)
   {
      *error = fix_error_create(FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      goto err;
   }
   if (xml_validate(doc, error) == FIX_FAILED)
   {
      goto err;
   }
   xmlNode* root = xmlDocGetRootElement(doc);
   prot->transportVersion = _strdup(get_attr(root, "version", NULL));
   if (!strcmp(prot->version, prot->transportVersion)) // versions are the same, no need to process transport protocol
   {
      goto ok;
   }
   if (!root)
   {
      *error = fix_error_create(FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      goto err;
   }
   if (load_field_types(&prot->transport_field_types, root, error) == FIX_FAILED)
   {
      goto err;
   }
   if (load_messages(prot, &prot->transport_field_types, root, error) == FIX_FAILED)
   {
      goto err;
   }
   goto ok;
err:
   res = FIX_FAILED;
ok:
   if (doc)
   {
      xmlFreeDoc(doc);
   }
   return res;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
/* PUBLICS                                                                                                               */
/*-----------------------------------------------------------------------------------------------------------------------*/
FIXProtocolDescr const* fix_protocol_descr_create(char const* file, FIXError** error)
{
   FIXProtocolDescr* prot = NULL;
   initLibXml(error);
   xmlDoc* doc = xmlParseFile(file);
   if (!doc)
   {
      *error = fix_error_create(FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      goto err;
   }
   if (xml_validate(doc, error) == FIX_FAILED)
   {
      goto err;
   }
   xmlNode* root = xmlDocGetRootElement(doc);
   if (!root)
   {
      *error = fix_error_create(FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      goto err;
   }
   prot = (FIXProtocolDescr*)calloc(1, sizeof(FIXProtocolDescr));
   prot->version = _strdup(get_attr(root, "version", NULL));
   if (prot && load_transport_protocol(prot, root, file, error) == FIX_FAILED)
   {
      goto err;
   }
   else if (load_field_types(&prot->field_types, root, error) == FIX_FAILED)
   {
      goto err;
   }
   else if (load_messages(prot, &prot->field_types, root, error) == FIX_FAILED)
   {
      goto err;
   }
   goto ok;
err:
   if (prot)
   {
      free(prot);
      prot = NULL;
   }
ok:
   if (doc)
   {
      xmlFreeDoc(doc);
   }
   return prot;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
void fix_protocol_descr_free(FIXProtocolDescr const* prot)
{
   if (!prot)
   {
      return;
   }
   for(int32_t i = 0; i < FIELD_TYPE_CNT; ++i)
   {
      FIXFieldType const* ft = prot->field_types[i];
      while(ft)
      {
         FIXFieldType* next_ft = ft->next;
         free_field_type(ft);
         ft = next_ft;
      }
   }
   for(int32_t i = 0; i < MSG_CNT; ++i)
   {
      FIXMsgDescr const* msg = prot->messages[i];
      while(msg)
      {
         FIXMsgDescr* next_msg = msg->next;
         free_message(msg);
         msg = next_msg;
      }
   }
   free(prot->version);
   free(prot->transportVersion);
   free((void*)prot);
}


/*-----------------------------------------------------------------------------------------------------------------------*/
FIXFieldType* fix_protocol_get_field_type(FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], char const* name)
{
   int32_t idx = fix_utils_hash_string(name, strlen(name)) % FIELD_TYPE_CNT;
   FIXFieldType* fld = (*ftypes)[idx];
   while(fld)
   {
      if (!strcmp(fld->name, name))
      {
         return fld;
      }
      fld = fld->next;
   }
   return NULL;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
FIXMsgDescr const* fix_protocol_get_msg_descr(FIXParser* parser, char const* type, FIXError** error)
{
   int32_t idx = fix_utils_hash_string(type, strlen(type)) % MSG_CNT;
   FIXMsgDescr* msg = parser->protocol->messages[idx];
   while(msg)
   {
      if (!strcmp(msg->type, type))
      {
         return msg;
      }
      msg = msg->next;
   }
   *error = fix_error_create(FIX_ERROR_UNKNOWN_MSG, "FIXMsgDescr with type '%s' not found", type);
   return NULL;
}

//------------------------------------------------------------------------------------------------------------------------//
FIXFieldDescr const* fix_protocol_get_field_descr(FIXMsgDescr const* msg, FIXTagNum tag)
{
   int32_t idx = tag % FIELD_DESCR_CNT;
   FIXFieldDescr const* fld = msg->field_index[idx];
   while(fld)
   {
      if (fld->type->tag == tag)
      {
         return fld;
      }
      fld = fld->next;
   }
   return 0;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
FIXFieldDescr const* fix_protocol_get_group_descr(FIXFieldDescr const* field, FIXTagNum tag)
{
   int32_t idx = tag % FIELD_DESCR_CNT;
   FIXFieldDescr const* fld = field->group_index[idx];
   while(fld)
   {
      if (fld->type->tag == tag)
      {
         return fld;
      }
      fld = fld->next;
   }
   return NULL;
}

/*------------------------------------------------------------------------------------------------------------------------*/
FIXFieldDescr const* fix_protocol_get_descr(FIXMsg* msg, FIXGroup const* group, FIXTagNum tag, FIXError** error)
{
   FIXFieldDescr const* fdescr = NULL;
   if (group)
   {
      fdescr = fix_protocol_get_group_descr(group->parent_fdescr, tag);
      if (!fdescr)
      {
         *error = fix_error_create(FIX_ERROR_UNKNOWN_FIELD, "Field with tag %d not found in group '%s' description.",
               tag, group->parent_fdescr->type->name);
      }
   }
   else
   {
      fdescr = fix_protocol_get_field_descr(msg->descr, tag);
      if (!fdescr)
      {
         *error = fix_error_create(FIX_ERROR_UNKNOWN_FIELD, "Field with tag %d not found in message '%s' description.",
               tag, msg->descr->name);
      }
   }
   return fdescr;
}

//------------------------------------------------------------------------------------------------------------------------//
int32_t fix_protocol_check_field_value(FIXFieldDescr const* fdescr, char const* value, uint32_t len)
{
   FIXFieldValue** values = fdescr->type->values;
   if (!values) // no values at all, value is correct
   {
      return 1;
   }
   uint32_t idx = fix_utils_hash_string(value, len) % FIELD_VALUE_CNT;
   FIXFieldValue* it = values[idx];
   while(it)
   {
      if (!strncmp(it->value, value, len))
      {
         return 1; // found and equal, so correct
      }
      it = it->next;
   }
   return 0; // nothing was found, so incorrect
}
