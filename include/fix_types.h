/* @file   fix_types.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/31/2012 10:22:19 AM
*/

#ifndef FIX_PARSER_FIX_TYPES_H
#define FIX_PARSER_FIX_TYPES_H

#define FIX_FAILED                       -1
#define FIX_SUCCESS                       0
#define FIX_ERROR_TAG_HAS_WRONG_TYPE      1
#define FIX_ERROR_TAG_NOT_FOUND           2
#define FIX_ERROR_GROUP_WRONG_INDEX       3
#define FIX_ERROR_XML_ATTR_NOT_FOUND      4
#define FIX_ERROR_XML_ATTR_WRONG_VALUE    5
#define FIX_ERROR_PROTOCOL_XML_LOAD_FAILED 6
#define FIX_ERROR_UNKNOWN_FIELD           7
#define FIX_ERROR_WRONG_PROTOCOL_VER      8
#define FIX_ERROR_DUPLICATE_FIELD_DESCR   9
#define FIX_ERROR_UNKNOWN_MSG            10
#define FIX_ERROR_LIBXML                 11
#define FIX_ERROR_INVALID_ARGUMENT       12
#define FIX_ERROR_MALLOC                 13
#define FIX_ERROR_UNKNOWN_PROTOCOL_DESCR 14
#define FIX_ERROR_NO_MORE_PAGES          15
#define FIX_ERROR_NO_MORE_TABLES         16

typedef struct FIXTagTable_ FIXGroup;
typedef struct FIXTag_ FIXTag;
typedef struct FIXMessage_ FIXMessage;
typedef struct FIXParser_ FIXParser;

typedef enum FIXTagType_
{
   FIXTagType_Value = 1,
   FIXTagType_Group = 2
} FIXTagType;

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

#define IS_STRING_TYPE(type) \
   (type == FIXFieldType_String || type == FIXFieldType_MultipleValueString || type == FIXFieldType_Country || \
    type == FIXFieldType_Currency || type == FIXFieldType_Exchange || type == FIXFieldType_MonthYear || \
    type == FIXFieldType_UTCTimestamp || type == FIXFieldType_UTCTimeOnly || type == FIXFieldType_UTCTimeOnly || \
    type == FIXFieldType_UTCDateOnly || type == FIXFieldType_LocalMktDate || type == FIXFieldType_TZTimeOnly || \
    type == FIXFieldType_TZTimestamp)

#define IS_INT_TYPE(type) \
   (type == FIXFieldType_Int || type == FIXFieldType_Length || type == FIXFieldType_NumInGroup || type == FIXFieldType_SeqNum || \
    type == FIXFieldType_TagNum || type == FIXFieldType_DayOfMonth)

#define IS_FLOAT_TYPE(type) \
   (type == FIXFieldType_Float || type == FIXFieldType_Qty || type == FIXFieldType_Price || type == FIXFieldType_PriceOffset || \
    type == FIXFieldType_Amt || type == FIXFieldType_Percentage)

#define IS_CHAR_TYPE(type) \
   (type == FIXFieldType_Char || type == FIXFieldType_Boolean)

FIXProtocolVerEnum str2FIXProtocolVerEnum(char const* ver);
FIXFieldTypeEnum str2FIXFIXFieldType(char const* type);

#endif /* FIX_PARSER_FIX_TYPES_H */
