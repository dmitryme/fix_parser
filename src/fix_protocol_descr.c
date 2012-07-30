/// @file   fix_protocol_descr.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/27/2012 06:14:53 PM

#include "fix_protocol_descr.h"
#include "fix_error.h"
#include "fix_utils.h"

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <string.h>
#include <stdint.h>

extern void set_fix_va_error(int, char const*, va_list ap);
extern void set_fix_error(int, char const*, ...);

extern char const* fix_xsd;

void xmlErrorHandler(void* ctx, char const* msg, ...)
{
   va_list ap;
   va_start(ap, msg);
   set_fix_va_error(FIX_ERROR_LIBXML, msg, ap);
   va_end(ap);
}

int initLibXml()
{
   xmlSetGenericErrorFunc(0, xmlErrorHandler);
   return 0;
}

int xml_validate(xmlDoc* doc)
{
   xmlSchemaParserCtxtPtr pctx = xmlSchemaNewMemParserCtxt(fix_xsd, strlen(fix_xsd));
   xmlSchemaPtr schema = xmlSchemaParse(pctx);
   if (!schema)
   {
      return FIX_FAILED;
   }
   xmlSchemaValidCtxtPtr validCtx = xmlSchemaNewValidCtxt(schema);
   xmlSchemaSetValidErrors(validCtx, &xmlErrorHandler, &xmlErrorHandler, validCtx);

   int res = xmlSchemaValidateDoc(validCtx, doc);

   xmlSchemaFreeValidCtxt(validCtx);
   xmlSchemaFree(schema);
   xmlSchemaFreeParserCtxt(pctx);

   return res ? FIX_FAILED : FIX_SUCCESS;
}

FIXProtocolDescr* protocols[FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT - 1];

char const* get_attr(xmlNode const* node, char const* attrName)
{
   if (!node)
   {
      return NULL;
   }
   xmlAttr const* attr = node->properties;
   while(attr)
   {
      if (!strcmp(attr->name, attrName))
      {
         return attr->children->content;
      }
      attr = attr->next;
   }
   return NULL;
}

xmlNode* get_first(xmlNode const* node, char const* name)
{
   xmlNode* child = node->children;
   while(child)
   {
      if (child->type == XML_ELEMENT_NODE && !strcmp(child->name, name))
      {
         return child;
      }
      child = child->next;
   }
   return NULL;
}

FIXProtocolVerEnum get_version(xmlNode const* root)
{
   char const* ver = get_attr(root, "version");
   if (!strcmp(ver, "FIX42")) return FIX42;
   if (!strcmp(ver, "FIX44")) return FIX44;
   if (!strcmp(ver, "FIX50")) return FIX50;
   if (!strcmp(ver, "FIX50SP1")) return FIX50SP1;
   if (!strcmp(ver, "FIX50SP2")) return FIX50SP2;
   if (!strcmp(ver, "FIXT11")) return FIXT11;
   return FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT;
}

FIXFieldTypeEnum string2FIXFIXFieldType(char const* type)
{
   if (!strcmp(type, "Int"))           { return FIXFieldType_Int; }
   if (!strcmp(type, "Length"))        { return FIXFieldType_Length; }
   if (!strcmp(type, "NumInGroup"))    { return FIXFieldType_NumInGroup; }
   if (!strcmp(type, "SeqNum"))        { return FIXFieldType_SeqNum; }
   if (!strcmp(type, "TagNum"))        { return FIXFieldType_TagNum; }
   if (!strcmp(type, "DayOfMonth"))    { return FIXFieldType_DayOfMonth; }
   if (!strcmp(type, "Float"))         { return FIXFieldType_Float; }
   if (!strcmp(type, "Qty"))           { return FIXFieldType_Qty; }
   if (!strcmp(type, "Price"))         { return FIXFieldType_Price; }
   if (!strcmp(type, "PriceOffset"))   { return FIXFieldType_PriceOffset; }
   if (!strcmp(type, "Amt"))           { return FIXFieldType_Amt; }
   if (!strcmp(type, "Percentage"))    { return FIXFieldType_Percentage; }
   if (!strcmp(type, "Char"))          { return FIXFieldType_Char; }
   if (!strcmp(type, "Boolean"))       { return FIXFieldType_Boolean; }
   if (!strcmp(type, "String"))        { return FIXFieldType_String; }
   if (!strcmp(type, "MultipleValueString")) { return FIXFieldType_MultipleValueString; }
   if (!strcmp(type, "Country"))       { return FIXFieldType_Country; }
   if (!strcmp(type, "Currency"))      { return FIXFieldType_Currency; }
   if (!strcmp(type, "Exchange"))      { return FIXFieldType_Exchange; }
   if (!strcmp(type, "MonthYear"))     { return FIXFieldType_MonthYear; }
   if (!strcmp(type, "UTCTimestamp"))  { return FIXFieldType_UTCTimestamp; }
   if (!strcmp(type, "UTCTimeOnly"))   { return FIXFieldType_UTCTimeOnly; }
   if (!strcmp(type, "UTCDateOnly"))   { return FIXFieldType_UTCDateOnly; }
   if (!strcmp(type, "LocalMktDate"))  { return FIXFieldType_LocalMktDate; }
   if (!strcmp(type, "Data"))          { return FIXFieldType_Data; }
   if (!strcmp(type, "TZTimeOnly"))    { return FIXFieldType_TZTimeOnly; }
   if (!strcmp(type, "TZTimestamp"))   { return FIXFieldType_TZTimestamp; }
   if (!strcmp(type, "XMLData"))       { return FIXFieldType_XMLData; }
   if (!strcmp(type, "Language"))      { return FIXFieldType_Language; }
   return FIXFieldType_Unknown;
}

void free_field_descr(FIXFieldDescr* fd)
{
   for(uint32_t i = 0; i < fd->group_count; ++i)
   {
      free_field_descr(&fd->group[i]);
   }
   free(fd->group);
   free(fd->group_index);
}

void free_field_type(FIXFieldType* ft)
{
   free(ft->name);
   free(ft);
}

void free_message(FIXMessageDescr* msg)
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

void free_protocol(FIXProtocolDescr* prot)
{
   if (!prot)
   {
      return;
   }
   for(int i = 0; i < FIELD_TYPE_CNT; ++i)
   {
      FIXFieldType* ft = prot->field_types[i];
      while(ft)
      {
         FIXFieldType* next_ft = ft->next;
         free_field_type(ft);
         ft = next_ft;
      }
   }
   for(int i = 0; i < MSG_CNT; ++i)
   {
      FIXMessageDescr* msg = prot->messages[i];
      while(msg)
      {
         FIXMessageDescr* next_msg = msg->next;
         free_message(msg);
         msg = next_msg;
      }
   }
}

int load_field_types(FIXProtocolDescr* prot, xmlNode const* root)
{
   xmlNode const* field = get_first(get_first(root, "fields"), "field");
   while(field)
   {
      if (field->type == XML_ELEMENT_NODE && !strcmp(field->name, "field"))
      {
         if (get_fix_field_type(prot, field->name))
         {
            set_fix_error(FIX_ERROR_DUPLICATE_FIELD_DESCR, "Field type '%s' already exists", field->name);
            return FIX_FAILED;
         }
         FIXFieldType* fld = (FIXFieldType*)malloc(sizeof(FIXFieldType));
         fld->num = atoi(get_attr(field, "number"));
         char const* name = get_attr(field, "name");
         fld->name = (char*)malloc(strlen(name) + 1);
         strcpy(fld->name, name);
         fld->type = string2FIXFIXFieldType(get_attr(field, "type"));
         int idx = hash_string(fld->name) % FIELD_TYPE_CNT;
         fld->next = prot->field_types[idx];
         prot->field_types[idx] = fld;
      }
      field = field->next;
   }
   return FIX_SUCCESS;
}

int load_fields(FIXFieldDescr** fields, uint32_t* count, xmlNode const* msg_node, xmlNode const* root, FIXProtocolDescr const* prot)
{
   xmlNode const* field = msg_node->children;
   while(field)
   {
      if (field->type == XML_ELEMENT_NODE && !strcmp(field->name, "field"))
      {
         char const* name = get_attr(field, "name");
         char const* required = get_attr(field, "required");
         *fields = realloc(*fields, ++(*count) * sizeof(FIXFieldDescr));
         FIXFieldDescr* fld = &(*fields)[*count - 1];
         memset(fld, 0, sizeof(FIXFieldDescr));
         fld->field_type = get_fix_field_type(prot, name);
         if (!fld->field_type)
         {
            return FIX_FAILED;
         }
         if (!strcmp(required, "Y"))
         {
            fld->flags |= FIELD_FLAG_REQUIRED;
         }
      }
      else if (field->type == XML_ELEMENT_NODE && !strcmp(field->name, "component"))
      {
         char const* component_name = get_attr(field, "name");
         xmlNode* component = get_first(get_first(root, "components"), "component");
         while(component)
         {
           if (component->type == XML_ELEMENT_NODE)
           {
              char const* name = get_attr(component, "name");
              if (!strcmp(component_name, name))
              {
                 if (FIX_FAILED == load_fields(fields, count, component, root, prot))
                 {
                    return FIX_FAILED;
                 }
              }
           }
           component = component->next;
         }
      }
      else if (field->type == XML_ELEMENT_NODE && !strcmp(field->name, "group"))
      {
         char const* name = get_attr(field, "name");
         char const* required = get_attr(field, "required");
         *fields = realloc(*fields, ++(*count) * sizeof(FIXFieldDescr));
         FIXFieldDescr* fld = &(*fields)[*count - 1];
         memset(fld, 0, sizeof(FIXFieldDescr));
         fld->field_type = get_fix_field_type(prot, name);
         if (!fld->field_type)
         {
          return 0;
         }
         if (!strcmp(required, "Y"))
         {
          fld->flags |= FIELD_FLAG_REQUIRED;
         }
         fld->group_index = calloc(FIELD_DESCR_CNT, sizeof(FIXFieldDescr*));
         if (FIX_FAILED == load_fields(&fld->group, &fld->group_count, field, root, prot))
         {
          return 0;
         }
      }
      field = field->next;
   }
   return FIX_SUCCESS;
}

void build_index(FIXFieldDescr* fields, uint32_t field_count, FIXFieldDescr** index)
{
   for(uint32_t i = 0; i < field_count; ++i)
   {
      FIXFieldDescr* fld = &fields[i];
      int idx = fld->field_type->num % FIELD_DESCR_CNT;
      fld->next = index[idx];
      index[idx] = fld;
      if (fld->group_count)
      {
         build_index(fld->group, fld->group_count, fld->group_index);
      }
   }
}

FIXMessageDescr* load_message(xmlNode const* msg_node, xmlNode const* root, FIXProtocolDescr const* prot)
{
   FIXMessageDescr* msg = (FIXMessageDescr*)calloc(1, sizeof(FIXMessageDescr));
   char const* name = get_attr(msg_node, "name");
   msg->name = malloc(strlen(name) + 1);
   strcpy(msg->name, name);
   char const* type = get_attr(msg_node, "type");
   msg->type = malloc(strlen(type) + 1);
   strcpy(msg->type, type);
   if (FIX_FAILED == load_fields(&msg->fields, &msg->field_count, msg_node, root, prot))
   {
      return NULL;
   }
   msg->field_index = calloc(FIELD_DESCR_CNT, sizeof(FIXFieldDescr*));
   build_index(msg->fields, msg->field_count, msg->field_index);
   return msg;
}

FIXProtocolDescr* fix_protocol_descr_init(char const* file)
{
   initLibXml();
   xmlDoc* doc = xmlParseFile(file);
   if (!doc)
   {
      set_fix_error(FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      return NULL;
   }
   if (xml_validate(doc) == FIX_FAILED)
   {
      xmlFreeDoc(doc);
      return NULL;
   }
   xmlNode* root = xmlDocGetRootElement(doc);
   if (!root)
   {
      set_fix_error(FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      return NULL;
   }
   FIXProtocolDescr* prot = calloc(1, sizeof(FIXProtocolDescr));
   prot->version = get_version(root);
   if (load_field_types(prot, root) == FIX_FAILED)
   {
      free(prot);
      return NULL;
   }
   xmlNode* msg_node = get_first(get_first(root, "messages"), "message");
   while(msg_node)
   {
      if (msg_node->type == XML_ELEMENT_NODE && !strcmp(msg_node->name, "message"))
      {
         FIXMessageDescr* msg = load_message(msg_node, root, prot);
         if (!msg)
         {
            free(prot);
            return NULL;
         }
         int idx = hash_string(msg->type) % MSG_CNT;
         msg->next = prot->messages[idx];
         prot->messages[idx] = msg;
      }
      msg_node = msg_node->next;
   }
   if (protocols[prot->version])
   {
      free_protocol(protocols[prot->version]);
   }
   protocols[prot->version] = prot;
   xmlFreeDoc(doc);
   return prot;
}

FIXProtocolDescr* get_fix_protocol_descr(FIXProtocolVerEnum ver)
{
   if (ver < 0 || ver >= FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT)
   {
      set_fix_error(FIX_ERROR_WRONG_PROTOCOL_VER, "Wrong FIX protocol version %d", ver);
      return NULL;
   }
   return protocols[ver];
}

void free_protocols()
{
   for(int i = 0; i < FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT; ++i)
   {
      free_protocol(protocols[i]);
   }
}

FIXFieldType* get_fix_field_type(FIXProtocolDescr const* prot, char const* name)
{
   int idx = hash_string(name) % FIELD_TYPE_CNT;
   FIXFieldType* fld = prot->field_types[idx];
   while(fld)
   {
      if (!strcmp(fld->name, name))
      {
         return fld;
      }
      fld = fld->next;
   }
   set_fix_error(FIX_ERROR_UNKNOWN_FIELD, "FIXFieldType '%s' is unknown", name);
   return NULL;
}

FIXMessageDescr* get_fix_message_descr(FIXProtocolDescr const* prot, char const* type)
{
   int idx = hash_string(type) % MSG_CNT;
   FIXMessageDescr* msg = prot->messages[idx];
   while(msg)
   {
      if (!strcmp(msg->type, type))
      {
         return msg;
      }
   }
   set_fix_error(FIX_ERROR_UNKNOWN_MSG, "FIXMessageDescr with type '%s' not found", type);
   return NULL;
}

FIXFieldDescr* get_fix_field_descr(FIXMessageDescr const* msg, uint32_t num)
{
   int idx = num % FIELD_DESCR_CNT;
   FIXFieldDescr* fld = msg->field_index[idx];
   while(fld)
   {
      if (fld->field_type->num == num)
      {
         return fld;
      }
   }
   set_fix_error(FIX_ERROR_UNKNOWN_FIELD, "Field with num %d not found in message '%s'", num, msg->name);
   return NULL;
}

FIXFieldDescr* get_fix_group_field_descr(FIXFieldDescr const* field, uint32_t num)
{
   int idx = num % FIELD_DESCR_CNT;
   FIXFieldDescr* fld = field->group_index[idx];
   while(fld)
   {
      if (fld->field_type->num == num)
      {
         return fld;
      }
   }
   set_fix_error(FIX_ERROR_UNKNOWN_FIELD, "Field with num %d not found in group '%s'", num, field->field_type->name);
   return NULL;
}