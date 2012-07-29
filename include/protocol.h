/// @file   protocol.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/24/2012 06:36:13 PM

#include <stdint.h>

#define FIELD_TYPE_CNT 1000
#define FIELD_DESCR_CNT 100
#define MSG_CNT   100
#define FIELD_FLAG_REQUIRED 0x01

typedef enum FIXProtocolVer_
{
   FIX42,
   FIX44,
   FIX50,
   FIX50SP1,
   FIX50SP2,
   FIXT11,
   FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT
} FIXProtocolVer;

typedef enum FIXFieldType_
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
} FIXFieldType;

typedef struct FieldType_
{
   uint32_t num;
   FIXFieldType type;
   char* name;
   struct FieldType_* next;
} FieldType;

typedef struct FieldDescr_
{
   FieldType* field_type;
   uint8_t flags;
   uint32_t group_count;
   struct FieldDescr_* group;
   struct FieldDescr_** group_index;
   struct FieldDescr_* next;
} FieldDescr;

typedef struct MessageDescr_
{
   char* type;
   char* name;
   uint32_t field_count;
   FieldDescr* fields;
   FieldDescr** field_index;
   struct MessageDescr_* next;
} MessageDescr;

typedef struct Protocol_
{
   FIXProtocolVer version;
   FieldType* field_types[FIELD_TYPE_CNT];
   MessageDescr* messages[MSG_CNT];
} Protocol;

Protocol* protocol_init(char const* file);
Protocol* get_protocol(FIXProtocolVer version);
void free_protocols();

FieldType* get_field_type_by_name(Protocol const* prot, char const* name);
MessageDescr* get_message_by_type(Protocol const* prot, char const* type);
FieldDescr* get_field_descr_by_num(MessageDescr const* msg, uint32_t num);
FieldDescr* get_group_field_descr_by_num(FieldDescr const* field, uint32_t num);