/* @file   fix_types.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/31/2012 10:22:19 AM
*/

#ifndef FIX_PARSER_FIX_TYPES_H
#define FIX_PARSER_FIX_TYPES_H

#include "fix_tag_num.h"

#define FIX_FAILED                        -1
#define FIX_SUCCESS                        0
#define FIX_ERROR_TAG_HAS_WRONG_TYPE       1
#define FIX_ERROR_TAG_NOT_FOUND            2
#define FIX_ERROR_GROUP_WRONG_INDEX        3
#define FIX_ERROR_XML_ATTR_NOT_FOUND       4
#define FIX_ERROR_XML_ATTR_WRONG_VALUE     5
#define FIX_ERROR_PROTOCOL_XML_LOAD_FAILED 6
#define FIX_ERROR_UNKNOWN_FIELD            7
#define FIX_ERROR_WRONG_PROTOCOL_VER       8
#define FIX_ERROR_DUPLICATE_FIELD_DESCR    9
#define FIX_ERROR_UNKNOWN_MSG             10
#define FIX_ERROR_LIBXML                  11
#define FIX_ERROR_INVALID_ARGUMENT        12
#define FIX_ERROR_MALLOC                  13
#define FIX_ERROR_UNKNOWN_PROTOCOL_DESCR  14
#define FIX_ERROR_NO_MORE_PAGES           15
#define FIX_ERROR_NO_MORE_GROUPS          16
#define FIX_ERROR_TOO_BIG_PAGE            17

typedef struct FIXTagTable_ FIXGroup;
typedef struct FIXTag_ FIXTag;
typedef struct FIXMsg_ FIXMsg;
typedef struct FIXParser_ FIXParser;

typedef enum FixParserFlagEnum_
{
   FixParserFlags_CheckCRC = 0x01,
   FIXParserFlags_Validate = 0x02
} FIXParserFlagEnum;

typedef enum FIXTagTypeEnum_
{
   FIXTagType_Value = 1,
   FIXTagType_Group = 2
} FIXTagTypeEnum;

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
   FIXFieldType_Int                  = 0x01,
   FIXFieldType_Length               = 0x02,
   FIXFieldType_NumInGroup           = 0x03,
   FIXFieldType_SeqNum               = 0x04,
   FIXFieldType_TagNum               = 0x05,
   FIXFieldType_DayOfMonth           = 0x06,
   FIXFieldType_Float                = 0x10,
   FIXFieldType_Qty                  = 0x20,
   FIXFieldType_Price                = 0x30,
   FIXFieldType_PriceOffset          = 0x40,
   FIXFieldType_Amt                  = 0x50,
   FIXFieldType_Percentage           = 0x60,
   FIXFieldType_Char                 = 0x100,
   FIXFieldType_Boolean              = 0x200,
   FIXFieldType_String               = 0x1000,
   FIXFieldType_MultipleValueString  = 0x2000,
   FIXFieldType_Country              = 0x3000,
   FIXFieldType_Currency             = 0x4000,
   FIXFieldType_Exchange             = 0x5000,
   FIXFieldType_MonthYear            = 0x6000,
   FIXFieldType_UTCTimestamp         = 0x7000,
   FIXFieldType_UTCTimeOnly          = 0x8000,
   FIXFieldType_UTCDateOnly          = 0x9000,
   FIXFieldType_LocalMktDate         = 0xA000,
   FIXFieldType_TZTimeOnly           = 0xB000,
   FIXFieldType_TZTimestamp          = 0xC000,
   FIXFieldType_Language             = 0xD000,
   FIXFieldType_XMLData              = 0x20000,
   FIXFieldType_Data                 = 0x10000
} FIXFieldTypeEnum;

#define IS_STRING_TYPE(type) ((type & 0xF000) > 0)
#define IS_INT_TYPE(type)    ((type & 0x0F) > 0)
#define IS_FLOAT_TYPE(type)  ((type & 0xF0) > 0)
#define IS_CHAR_TYPE(type)   ((type & 0xF00) > 0)
#define IS_DATA_TYPE(type)   ((type & 0xF0000) > 0)

FIXProtocolVerEnum str2FIXProtocolVerEnum(char const* ver);
char const* FIXProtocolVerEnum2BeginString(FIXProtocolVerEnum ver);
FIXFieldTypeEnum str2FIXFIXFieldType(char const* type);

#endif /* FIX_PARSER_FIX_TYPES_H */