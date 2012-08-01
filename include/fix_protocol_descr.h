/// @file   fix_protocol_descr.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/24/2012 06:36:13 PM

#ifndef FIX_PARSER_FIX_PROTOCOL_DESCR_H
#define FIX_PARSER_FIX_PROTOCOL_DESCR_H

#include "fix_types.h"

#include <stdint.h>

#define FIELD_TYPE_CNT 1000
#define FIELD_DESCR_CNT 100
#define MSG_CNT   100
#define FIELD_FLAG_REQUIRED 0x01

typedef struct FIXFieldType_
{
   uint32_t num;
   FIXFieldTypeEnum type;
   char* name;
   struct FIXFieldType_* next;
} FIXFieldType;

typedef struct FIXFieldDescr_
{
   FIXFieldType* field_type;
   uint8_t flags;
   uint32_t group_count;
   struct FIXFieldDescr_*  group;
   struct FIXFieldDescr_** group_index;
   struct FIXFieldDescr_*  next;
} FIXFieldDescr;

typedef struct FIXMessageDescr_
{
   char* type;
   char* name;
   uint32_t field_count;
   FIXFieldDescr* fields;
   FIXFieldDescr** field_index;
   struct FIXMessageDescr_* next;
} FIXMessageDescr;

typedef struct FIXProtocolDescr_
{
   FIXProtocolVerEnum version;
   FIXFieldType* field_types[FIELD_TYPE_CNT];
   FIXMessageDescr* messages[MSG_CNT];
} FIXProtocolDescr;

FIXProtocolDescr* fix_protocol_descr_init(char const* file);
FIXProtocolDescr* get_fix_protocol_descr(FIXProtocolVerEnum ver);
void free_protocol_descr();

FIXFieldType* get_fix_field_type(FIXProtocolDescr const* prot, char const* name);
FIXMessageDescr* get_fix_message_descr(FIXProtocolDescr const* prot, char const* type);
FIXFieldDescr* get_fix_field_descr(FIXMessageDescr const* msg, uint32_t num);
FIXFieldDescr* get_fix_group_field_descr(FIXFieldDescr const* field, uint32_t num);

#endif // FIX_PARSER_FIX_PROTOCOL_DESCR_H