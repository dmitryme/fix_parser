/// @file   fix_protocol_descr.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/24/2012 06:36:13 PM

#include <stdint.h>

#define FIELD_TYPE_CNT 1000
#define FIELD_DESCR_CNT 100
#define MSG_CNT   100
#define FIELD_FLAG_REQUIRED 0x01

typedef enum FIXProtocolVerEnum_
{
   FIX42,
   FIX44,
   FIX50,
   FIX50SP1,
   FIX50SP2,
   FIXT11,
   FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT
} FIXProtocolVerEnum;

typedef enum FIXFieldTypeEnum_
{
   FIXFieldType_Unknown              = -1,
   FIXFieldType_Int                  = 1,
   FIXFieldType_Length               = 2,
   FIXFieldType_NumInGroup           = 3,
   FIXFieldType_SeqNum               = 4,
   FIXFieldType_TagNum               = 5,
   FIXFieldType_DayOfMonth           = 6,
   FIXFieldType_Float                = 7,
   FIXFieldType_Qty                  = 8,
   FIXFieldType_Price                = 9,
   FIXFieldType_PriceOffset          = 10,
   FIXFieldType_Amt                  = 11,
   FIXFieldType_Percentage           = 12,
   FIXFieldType_Char                 = 13,
   FIXFieldType_Boolean              = 14,
   FIXFieldType_String               = 15,
   FIXFieldType_MultipleValueString  = 16,
   FIXFieldType_Country              = 17,
   FIXFieldType_Currency             = 18,
   FIXFieldType_Exchange             = 19,
   FIXFieldType_MonthYear            = 20,
   FIXFieldType_UTCTimestamp         = 21,
   FIXFieldType_UTCTimeOnly          = 22,
   FIXFieldType_UTCDateOnly          = 23,
   FIXFieldType_LocalMktDate         = 24,
   FIXFieldType_Data                 = 25,
   FIXFieldType_TZTimeOnly           = 26,
   FIXFieldType_TZTimestamp          = 27,
   FIXFieldType_XMLData              = 28,
   FIXFieldType_Language             = 29
} FIXFieldTypeEnum;

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

typedef struct FIXProtocol_
{
   FIXProtocolVerEnum version;
   FIXFieldType* field_types[FIELD_TYPE_CNT];
   FIXMessageDescr* messages[MSG_CNT];
} FIXProtocolDescr;

FIXProtocolDescr* fix_protocol_descr_init(char const* file);
FIXProtocolDescr* get_fix_protocol_descr(FIXProtocolVerEnum version);
void free_protocol_descr();

FIXFieldType* get_fix_field_type(FIXProtocolDescr const* prot, char const* name);
FIXMessageDescr* get_fix_message_descr(FIXProtocolDescr const* prot, char const* type);
FIXFieldDescr* get_fix_field_descr(FIXMessageDescr const* msg, uint32_t num);
FIXFieldDescr* get_fix_group_field_descr(FIXFieldDescr const* field, uint32_t num);