/* @file   fix_protocol_descr.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/27/2012 06:14:53 PM
*/

#include "fix_protocol_descr.h"
#include "fix_utils.h"
#include "fix_types.h"
#include "fix_parser_priv.h"
#include "fix_descr_xsd.h"

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include  <libxml/tree.h>
#include <string.h>
#include <stdint.h>
#include  <libgen.h>
#include <limits.h>

/*-----------------------------------------------------------------------------------------------------------------------*/
/* PRIVATES                                                                                                              */
/*-----------------------------------------------------------------------------------------------------------------------*/
static void xmlErrorHandler(void* ctx, char const* msg, ...)
{
   va_list ap;
   va_start(ap, msg);
   fix_error_set_va((FIXError*)ctx, FIX_ERROR_LIBXML, msg, ap);
   va_end(ap);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static int32_t initLibXml(FIXError* error)
{
   xmlSetGenericErrorFunc(error, xmlErrorHandler);
   return 0;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static FIXErrCode xml_validate(FIXError* error, xmlDoc* doc)
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
static void free_field_type(FIXFieldType* ft)
{
   free(ft->name);
   free(ft);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static void free_message(FIXMsgDescr* msg)
{
   free(msg->name);
   free(msg->type);
   free(msg->field_index);
   for(uint32_t i = 0; i < msg->field_count; ++i)
   {
      free_field_descr(&msg->fields[i]);
   }
   free(msg->fields);
   free(msg);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static FIXErrCode load_field_types(FIXError* error, FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], xmlNode const* root)
{
   xmlNode const* field = get_first(get_first(root, "fields"), "field");
   while(field)
   {
      if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "field"))
      {
         if (fix_protocol_get_field_type(error, ftypes, get_attr(field, "name", NULL)))
         {
            fix_error_set(
                  error, FIX_ERROR_FIELD_TYPE_EXISTS, "FIXFieldType '%s' already exists", (char const*)field->name);
            return FIX_FAILED;
         }
         FIXFieldType* fld = (FIXFieldType*)malloc(sizeof(FIXFieldType));
         fld->tag = atoi(get_attr(field, "number", NULL));
         fld->name = strdup(get_attr(field, "name", NULL));
         fld->valueType = str2FIXFieldValueType(get_attr(field, "type", NULL));
         int32_t idx = fix_utils_hash_string(fld->name) % FIELD_TYPE_CNT;
         fld->next = (*ftypes)[idx];
         (*ftypes)[idx] = fld;
      }
      field = field->next;
   }
   return FIX_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static FIXErrCode load_fields(
      FIXError* error, FIXFieldDescr** fields, uint32_t* count, xmlNode const* msg_node, xmlNode const* root,
      FIXFieldType* (*ftypes)[FIELD_TYPE_CNT])
{
   xmlNode const* field = msg_node->children;
   FIXFieldDescr* prevFld = NULL;
   while(field)
   {
      if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "field"))
      {
         char const* name = get_attr(field, "name", NULL);
         char const* required = get_attr(field, "required", NULL);
         *fields = realloc(*fields, ++(*count) * sizeof(FIXFieldDescr));
         FIXFieldDescr* fld = &(*fields)[*count - 1];
         memset(fld, 0, sizeof(FIXFieldDescr));
         fld->type = fix_protocol_get_field_type(error, ftypes, name);
         fld->category = FIXFieldCategory_Value;
         if (!fld->type)
         {
            fix_error_set(error, FIX_ERROR_UNKNOWN_FIELD, "FIXFieldType '%s' is unknown", name);
            return FIX_FAILED;
         }
         if (!strcmp(required, "Y"))
         {
            fld->flags |= FIELD_FLAG_REQUIRED;
         }
         if (fld->type->valueType == FIXFieldValueType_Data)
         {
            if (prevFld->type->valueType != FIXFieldValueType_Length)
            {
               fix_error_set(error, FIX_ERROR_WRONG_FIELD, "Previous field for field '%s' shall have Length type.", name);
               return FIX_FAILED;
            }
            fld->dataLenField = prevFld;
         }
         prevFld = fld;
      }
      else if (field->type == XML_ELEMENT_NODE && !strcmp((char const*)field->name, "component"))
      {
         char const* component_name = get_attr(field, "name", NULL);
         xmlNode* component = get_first(get_first(root, "components"), "component");
         while(component)
         {
            if (component->type == XML_ELEMENT_NODE)
            {
               char const* name = get_attr(component, "name", NULL);
               if (!strcmp(component_name, name))
               {
                  if (FIX_FAILED == load_fields(error, fields, count, component, root, ftypes))
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
         *fields = realloc(*fields, ++(*count) * sizeof(FIXFieldDescr));
         FIXFieldDescr* fld = &(*fields)[*count - 1];
         memset(fld, 0, sizeof(FIXFieldDescr));
         fld->type = fix_protocol_get_field_type(error, ftypes, name);
         fld->category = FIXFieldCategory_Group;
         if (!fld->type)
         {
            fix_error_set(error, FIX_ERROR_UNKNOWN_FIELD, "FIXFieldType '%s' is unknown", name);
            return FIX_FAILED;
         }
         if (!strcmp(required, "Y"))
         {
            fld->flags |= FIELD_FLAG_REQUIRED;
         }
         fld->group_index = calloc(FIELD_DESCR_CNT, sizeof(FIXFieldDescr*));
         if (FIX_FAILED == load_fields(error, &fld->group, &fld->group_count, field, root, ftypes))
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
static FIXMsgDescr* load_message(FIXError* error, xmlNode const* msg_node, xmlNode const* root,
      FIXFieldType* (*ftypes)[FIELD_TYPE_CNT])
{
   FIXMsgDescr* msg = (FIXMsgDescr*)calloc(1, sizeof(FIXMsgDescr));
   msg->name = strdup(get_attr(msg_node, "name", NULL));
   msg->type = strdup(get_attr(msg_node, "type", NULL));
   if (FIX_FAILED == load_fields(error, &msg->fields, &msg->field_count, msg_node, root, ftypes))
   {
      return NULL;
   }
   msg->field_index = calloc(FIELD_DESCR_CNT, sizeof(FIXFieldDescr*));
   build_index(msg->fields, msg->field_count, msg->field_index);
   return msg;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static int32_t load_messages(FIXError* error, FIXProtocolDescr* prot, FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], xmlNode const* root)
{
   xmlNode* msg_node = get_first(get_first(root, "messages"), "message");
   while(msg_node)
   {
      if (msg_node->type == XML_ELEMENT_NODE && !strcmp((char const*)msg_node->name, "message"))
      {
         FIXMsgDescr* msg = load_message(error, msg_node, root, ftypes);
         if (!msg)
         {
            return FIX_FAILED;
         }
         int32_t idx = fix_utils_hash_string(msg->type) % MSG_CNT;
         msg->next = prot->messages[idx];
         prot->messages[idx] = msg;
      }
      msg_node = msg_node->next;
   }
   return FIX_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static int32_t load_transport_protocol(FIXError* error, FIXProtocolDescr* prot, xmlNode* parentRoot, char const* parentFile)
{
   char* transpFile = strdup(get_attr(parentRoot, "transport", parentFile));
   if(!strcmp(transpFile, parentFile)) // transport is the same as protocol
   {
      prot->transportVersion = strdup(prot->version);
      free(transpFile);
      return FIX_SUCCESS;
   }
   xmlDoc* doc = NULL;
   if (!strcmp(dirname(transpFile), "."))
   {
      char* parentFileDup = strdup(parentFile);
      char* parentPath = dirname(parentFileDup);
      char fullPath[PATH_MAX] = {};
      strcat(fullPath, parentPath);
      strcat(fullPath, "/");
      strcat(fullPath, basename(transpFile));
      doc = xmlParseFile(fullPath);
      free(parentFileDup);
   }
   else
   {
      doc = xmlParseFile(transpFile);
   }
   free(transpFile);
   if (!doc)
   {
      fix_error_set(error, FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      return FIX_FAILED;
   }
   if (xml_validate(error, doc) == FIX_FAILED)
   {
      xmlFreeDoc(doc);
      return FIX_FAILED;
   }
   xmlNode* root = xmlDocGetRootElement(doc);
   prot->transportVersion = strdup(get_attr(root, "version", NULL));
   if (!strcmp(prot->version, prot->transportVersion)) // versions are the same, no need to process transport protocol
   {
      xmlFreeDoc(doc);
      return FIX_SUCCESS;
   }
   if (!root)
   {
      fix_error_set(error, FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      xmlFreeDoc(doc);
      return FIX_FAILED;
   }
   if (load_field_types(error, &prot->transport_field_types, root) == FIX_FAILED)
   {
      xmlFreeDoc(doc);
      return FIX_FAILED;
   }
   if (load_messages(error, prot, &prot->transport_field_types, root) == FIX_FAILED)
   {
      xmlFreeDoc(doc);
      return FIX_FAILED;
   }
   xmlFreeDoc(doc);
   return FIX_SUCCESS;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
/* PUBLICS                                                                                                               */
/*-----------------------------------------------------------------------------------------------------------------------*/
FIXProtocolDescr* fix_protocol_descr_create(FIXError* error, char const* file)
{
   initLibXml(error);
   xmlDoc* doc = xmlParseFile(file);
   if (!doc)
   {
      fix_error_set(error, FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      return NULL;
   }
   if (xml_validate(error, doc) == FIX_FAILED)
   {
      xmlFreeDoc(doc);
      return NULL;
   }
   xmlNode* root = xmlDocGetRootElement(doc);
   if (!root)
   {
      fix_error_set(error, FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      xmlFreeDoc(doc);
      return NULL;
   }
   FIXProtocolDescr* prot = calloc(1, sizeof(FIXProtocolDescr));
   prot->version = strdup(get_attr(root, "version", NULL));
   if (prot && load_transport_protocol(error, prot, root, file) == FIX_FAILED)
   {
      free(prot);
      prot = NULL;
   }
   else if (load_field_types(error, &prot->field_types, root) == FIX_FAILED)
   {
      free(prot);
      prot = NULL;
   }
   else if (load_messages(error, prot, &prot->field_types, root) == FIX_FAILED)
   {
      free(prot);
      prot = NULL;
   }
   xmlFreeDoc(doc);
   return prot;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
void fix_protocol_descr_free(FIXProtocolDescr* prot)
{
   if (!prot)
   {
      return;
   }
   for(int32_t i = 0; i < FIELD_TYPE_CNT; ++i)
   {
      FIXFieldType* ft = prot->field_types[i];
      while(ft)
      {
         FIXFieldType* next_ft = ft->next;
         free_field_type(ft);
         ft = next_ft;
      }
   }
   for(int32_t i = 0; i < MSG_CNT; ++i)
   {
      FIXMsgDescr* msg = prot->messages[i];
      while(msg)
      {
         FIXMsgDescr* next_msg = msg->next;
         free_message(msg);
         msg = next_msg;
      }
   }
   free(prot->version);
   free(prot->transportVersion);
}


/*-----------------------------------------------------------------------------------------------------------------------*/
FIXFieldType* fix_protocol_get_field_type(FIXError* error, FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], char const* name)
{
   int32_t idx = fix_utils_hash_string(name) % FIELD_TYPE_CNT;
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
FIXMsgDescr* fix_protocol_get_msg_descr(FIXParser* parser, char const* type)
{
   int32_t idx = fix_utils_hash_string(type) % MSG_CNT;
   FIXMsgDescr* msg = parser->protocol->messages[idx];
   while(msg)
   {
      if (!strcmp(msg->type, type))
      {
         return msg;
      }
      msg = msg->next;
   }
   fix_error_set(&parser->error, FIX_ERROR_UNKNOWN_MSG, "FIXMsgDescr with type '%s' not found", type);
   return NULL;
}

//------------------------------------------------------------------------------------------------------------------------//
FIXFieldDescr* fix_protocol_get_field_descr(FIXError* error, FIXMsgDescr const* msg, FIXTagNum tag)
{
   int32_t idx = tag % FIELD_DESCR_CNT;
   FIXFieldDescr* fld = msg->field_index[idx];
   while(fld)
   {
      if (fld->type->tag == tag)
      {
         return fld;
      }
      fld = fld->next;
   }
   fix_error_set(error, FIX_ERROR_UNKNOWN_FIELD, "Field with tag %d not found in message '%s'", tag, msg->name);
   return 0;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
FIXFieldDescr* fix_protocol_get_group_descr(FIXError* error, FIXFieldDescr const* field, FIXTagNum tag)
{
   int32_t idx = tag % FIELD_DESCR_CNT;
   FIXFieldDescr* fld = field->group_index[idx];
   while(fld)
   {
      if (fld->type->tag == tag)
      {
         return fld;
      }
      fld = fld->next;
   }
   fix_error_set(error, FIX_ERROR_UNKNOWN_FIELD, "Field with tag %d not found in group '%s'", tag, field->type->name);
   return NULL;
}
