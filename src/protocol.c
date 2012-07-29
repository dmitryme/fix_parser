/// @file   protocol.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/27/2012 06:14:53 PM

#include "protocol.h"
#include "error.h"
#include "utils.h"

#include <libxml/parser.h>
#include <string.h>
#include <stdint.h>

extern void set_fix_error(int, char const*, ...);

Protocol* protocols[FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT - 1];

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

FIXProtocolVer get_version(xmlNode const* root)
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

FIXFieldType string2FIXFieldType(char const* type)
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

void free_field_descr(FieldDescr* fd)
{
   for(uint32_t i = 0; i < fd->group_count; ++i)
   {
      free_field_descr(&fd->group[i]);
   }
   free(fd->group);
   free(fd->group_index);
}

void free_field_type(FieldType* ft)
{
   free(ft->name);
   free(ft);
}

void free_message(MessageDescr* msg)
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

void free_protocol(Protocol* prot)
{
   if (!prot)
   {
      return;
   }
   for(int i = 0; i < FIELD_TYPE_CNT; ++i)
   {
      FieldType* ft = prot->field_types[i];
      while(ft)
      {
         FieldType* next_ft = ft->next;
         free_field_type(ft);
         ft = next_ft;
      }
   }
   for(int i = 0; i < MSG_CNT; ++i)
   {
      MessageDescr* msg = prot->messages[i];
      while(msg)
      {
         MessageDescr* next_msg = msg->next;
         free_message(msg);
         msg = next_msg;
      }
   }
}

int load_field_types(Protocol* prot, xmlNode const* root)
{
   xmlNode const* field = get_first(get_first(root, "fields"), "field");
   while(field)
   {
      if (field->type == XML_ELEMENT_NODE && !strcmp(field->name, "field"))
      {
         if (get_field_type_by_name(prot, field->name))
         {
            set_fix_error(FIX_ERROR_DUPLICATE_FIELD_DESCR, "Field type '%s' already exists", field->name);
            return FIX_FAILED;
         }
         FieldType* fld = (FieldType*)malloc(sizeof(FieldType));
         fld->num = atoi(get_attr(field, "number"));
         char const* name = get_attr(field, "name");
         fld->name = (char*)malloc(strlen(name) + 1);
         strcpy(fld->name, name);
         fld->type = string2FIXFieldType(get_attr(field, "type"));
         int idx = hash_string(fld->name) % FIELD_TYPE_CNT;
         fld->next = prot->field_types[idx];
         prot->field_types[idx] = fld;
      }
      field = field->next;
   }
   return FIX_SUCCESS;
}

int load_fields(FieldDescr** fields, uint32_t* count, xmlNode const* msg_node, xmlNode const* root, Protocol const* prot)
{
   xmlNode const* field = msg_node->children;
   while(field)
   {
      if (field->type == XML_ELEMENT_NODE && !strcmp(field->name, "field"))
      {
         char const* name = get_attr(field, "name");
         char const* required = get_attr(field, "required");
         *fields = realloc(*fields, ++(*count) * sizeof(FieldDescr));
         FieldDescr* fld = &(*fields)[*count - 1];
         memset(fld, 0, sizeof(FieldDescr));
         fld->field_type = get_field_type_by_name(prot, name);
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
         *fields = realloc(*fields, ++(*count) * sizeof(FieldDescr));
         FieldDescr* fld = &(*fields)[*count - 1];
         memset(fld, 0, sizeof(FieldDescr));
         fld->field_type = get_field_type_by_name(prot, name);
         if (!fld->field_type)
         {
          return 0;
         }
         if (!strcmp(required, "Y"))
         {
          fld->flags |= FIELD_FLAG_REQUIRED;
         }
         fld->group_index = calloc(FIELD_DESCR_CNT, sizeof(FieldDescr*));
         if (FIX_FAILED == load_fields(&fld->group, &fld->group_count, field, root, prot))
         {
          return 0;
         }
      }
      field = field->next;
   }
   return FIX_SUCCESS;
}

void build_index(FieldDescr* fields, uint32_t field_count, FieldDescr** index)
{
   for(uint32_t i = 0; i < field_count; ++i)
   {
      FieldDescr* fld = &fields[i];
      int idx = fld->field_type->num % FIELD_DESCR_CNT;
      fld->next = index[idx];
      index[idx] = fld;
      if (fld->group_count)
      {
         build_index(fld->group, fld->group_count, fld->group_index);
      }
   }
}

MessageDescr* load_message(xmlNode const* msg_node, xmlNode const* root, Protocol const* prot)
{
   MessageDescr* msg = (MessageDescr*)calloc(1, sizeof(MessageDescr));
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
   msg->field_index = calloc(FIELD_DESCR_CNT, sizeof(FieldDescr*));
   build_index(msg->fields, msg->field_count, msg->field_index);
   return msg;
}

Protocol* protocol_init(char const* file)
{
   xmlDoc* doc = xmlParseFile(file);
   if (!doc)
   {
      set_fix_error(FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      return NULL;
   }
   xmlNode* root = xmlDocGetRootElement(doc);
   if (!root)
   {
      set_fix_error(FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, xmlGetLastError()->message);
      return NULL;
   }
   Protocol* prot = calloc(1, sizeof(Protocol));
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
         MessageDescr* msg = load_message(msg_node, root, prot);
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
   xmlFreeDoc(doc);
   if (protocols[prot->version])
   {
      free_protocol(protocols[prot->version]);
   }
   protocols[prot->version] = prot;
   return prot;
}

Protocol* get_protocol(FIXProtocolVer ver)
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

FieldType* get_field_type_by_name(Protocol const* prot, char const* name)
{
   int idx = hash_string(name) % FIELD_TYPE_CNT;
   FieldType* fld = prot->field_types[idx];
   while(fld)
   {
      if (!strcmp(fld->name, name))
      {
         return fld;
      }
      fld = fld->next;
   }
   set_fix_error(FIX_ERROR_UNKNOWN_FIELD, "FieldType '%s' is unknown", name);
   return NULL;
}

MessageDescr* get_message_by_type(Protocol const* prot, char const* type)
{
   int idx = hash_string(type) % MSG_CNT;
   MessageDescr* msg = prot->messages[idx];
   while(msg)
   {
      if (!strcmp(msg->type, type))
      {
         return msg;
      }
   }
   set_fix_error(FIX_ERROR_UNKNOWN_MSG, "MessageDescr with type '%s' not found", type);
   return NULL;
}

FieldDescr* get_field_descr_by_num(MessageDescr const* msg, uint32_t num)
{
   int idx = num % FIELD_DESCR_CNT;
   FieldDescr* fld = msg->field_index[idx];
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

FieldDescr* get_group_field_descr_by_num(FieldDescr const* field, uint32_t num)
{
   int idx = num % FIELD_DESCR_CNT;
   FieldDescr* fld = field->group_index[idx];
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