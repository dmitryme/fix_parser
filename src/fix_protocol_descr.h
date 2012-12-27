/* @file   fix_protocol_descr.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/24/2012 06:36:13 PM
*/

#ifndef FIX_PARSER_FIX_PROTOCOL_DESCR_H
#define FIX_PARSER_FIX_PROTOCOL_DESCR_H

#include "fix_types.h"
#include "fix_error.h"

#include <stdint.h>

#pragma pack(push, 1)

#ifdef __cplusplus
extern "C"
{
#endif

#define FIELD_TYPE_CNT 1000
#define FIELD_DESCR_CNT 100
#define MSG_CNT   100
#define FIELD_FLAG_REQUIRED 0x01

typedef struct FIXFieldType_
{
   FIXTagNum tag;
   FIXFieldValueTypeEnum valueType;
   char* name;
   struct FIXFieldType_* next;
} FIXFieldType;

typedef struct FIXFieldDescr_
{
   FIXFieldType* type;
   FIXFieldCategoryEnum category;
   uint8_t flags;
   uint32_t group_count;
   struct FIXFieldDescr_*  group;
   struct FIXFieldDescr_** group_index;
   struct FIXFieldDescr_*  next;
   struct FIXFieldDescr_*  dataLenField;
} FIXFieldDescr;

typedef struct FIXMsgDescr_
{
   char* type;
   char* name;
   uint32_t field_count;
   FIXFieldDescr* fields;
   FIXFieldDescr** field_index;
   struct FIXMsgDescr_* next;
} FIXMsgDescr;

typedef struct FIXProtocolDescr_
{
   char* version;
   char* transportVersion;
   FIXFieldType* field_types[FIELD_TYPE_CNT];
   FIXFieldType* transport_field_types[FIELD_TYPE_CNT];
   FIXMsgDescr* messages[MSG_CNT];
} FIXProtocolDescr;

FIXProtocolDescr* fix_protocol_descr_create(FIXError* error, char const* file);
void fix_protocol_descr_free(FIXProtocolDescr* prot);

FIXFieldType* fix_protocol_get_field_type(FIXError* error, FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], char const* name);
FIXMsgDescr* fix_protocol_get_msg_descr(FIXParser* parser, char const* type);
FIXFieldDescr* fix_protocol_get_field_descr(FIXError* error, FIXMsgDescr const* msg, FIXTagNum tag);
FIXFieldDescr* fix_protocol_get_group_descr(FIXError* error, FIXFieldDescr const* field, FIXTagNum tag);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif /* FIX_PARSER_FIX_PROTOCOL_DESCR_H */
