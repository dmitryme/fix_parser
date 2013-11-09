/**
 * @file   fix_types.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/31/2012 10:22:19 AM
 */

#ifndef FIX_PARSER_FIX_TYPES_H
#define FIX_PARSER_FIX_TYPES_H

#include "fix_field_tag.h"

#include  <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define FIX_SOH                            0x01 ///< default FIX field delimiter

#define FIX_SUCCESS                           0
#define FIX_FAILED                           -1
#define FIX_ERROR_FIELD_HAS_WRONG_TYPE       -2
#define FIX_ERROR_FIELD_NOT_FOUND            -3
#define FIX_ERROR_FIELD_TYPE_EXISTS          -4
#define FIX_ERROR_GROUP_WRONG_INDEX          -5
#define FIX_ERROR_XML_ATTR_NOT_FOUND         -6
#define FIX_ERROR_XML_ATTR_WRONG_VALUE       -7
#define FIX_ERROR_PROTOCOL_XML_LOAD_FAILED   -8
#define FIX_ERROR_UNKNOWN_FIELD              -9
#define FIX_ERROR_WRONG_PROTOCOL_VER        -10
#define FIX_ERROR_DUPLICATE_FIELD_DESCR     -11
#define FIX_ERROR_UNKNOWN_MSG               -12
#define FIX_ERROR_LIBXML                    -13
#define FIX_ERROR_INVALID_ARGUMENT          -14
#define FIX_ERROR_MALLOC                    -15
#define FIX_ERROR_UNKNOWN_PROTOCOL_DESCR    -16
#define FIX_ERROR_NO_MORE_PAGES             -17
#define FIX_ERROR_NO_MORE_GROUPS            -18
#define FIX_ERROR_TOO_BIG_PAGE              -19
#define FIX_ERROR_NO_MORE_SPACE             -20
#define FIX_ERROR_PARSE_MSG                 -21
#define FIX_ERROR_WRONG_FIELD               -22
#define FIX_ERROR_INTEGRITY_CHECK           -23
#define FIX_ERROR_NO_MORE_DATA              -24
#define FIX_ERROR_WRONG_FIELD_VALUE         -25

typedef struct FIXGroup_ FIXGroup;
typedef struct FIXField_ FIXField;
typedef struct FIXMsg_ FIXMsg;
typedef struct FIXParser_ FIXParser;
typedef struct FIXError_ FIXError;
typedef int32_t FIXTagNum;  ///< FIX field tag type
typedef int32_t FIXErrCode; ///< error code

#define PARSER_FLAG_CHECK_CRC 0x01       ///< check FIX message CRC during parsing
#define PARSER_FLAG_CHECK_REQUIRED 0x02  ///< check for required FIX fields
#define PARSER_FLAG_CHECK_VALUE    0x04  ///< check for valid value.
#define PARSER_FLAG_CHECK_UNKNOWN_FIELDS 0x08 ///< check for unknown FIX fields during parsing. If not set all unknown fields ignored
#define PARSER_FLAG_CHECK_ALL \
   (PARSER_FLAG_CHECK_CRC | PARSER_FLAG_CHECK_REQUIRED | PARSER_FLAG_CHECK_VALUE | PARSER_FLAG_CHECK_UNKNOWN_FIELDS) ///< make all possible checks during parsing.

/**
 * Determine FIX field category (simple value or group of fields)
 */
typedef enum FIXFieldCategoryEnum
{
   FIXFieldCategory_Value = 1,  ///< FIX field is a simple value
   FIXFieldCategory_Group = 2   ///< FIX field is a group
} FIXFieldCategoryEnum;

/**
 * type of FIX field value
 */
typedef enum FIXFieldValueTypeEnum
{
   FIXFieldValueType_Unknown              = -1,
   FIXFieldValueType_Int                  = 0x01,
   FIXFieldValueType_Length               = 0x02,
   FIXFieldValueType_NumInGroup           = 0x03,
   FIXFieldValueType_SeqNum               = 0x04,
   FIXFieldValueType_TagNum               = 0x05,
   FIXFieldValueType_DayOfMonth           = 0x06,
   FIXFieldValueType_Float                = 0x10,
   FIXFieldValueType_Qty                  = 0x20,
   FIXFieldValueType_Price                = 0x30,
   FIXFieldValueType_PriceOffset          = 0x40,
   FIXFieldValueType_Amt                  = 0x50,
   FIXFieldValueType_Percentage           = 0x60,
   FIXFieldValueType_Char                 = 0x100,
   FIXFieldValueType_Boolean              = 0x200,
   FIXFieldValueType_String               = 0x1000,
   FIXFieldValueType_MultipleValueString  = 0x2000,
   FIXFieldValueType_Country              = 0x3000,
   FIXFieldValueType_Currency             = 0x4000,
   FIXFieldValueType_Exchange             = 0x5000,
   FIXFieldValueType_MonthYear            = 0x6000,
   FIXFieldValueType_UTCTimestamp         = 0x7000,
   FIXFieldValueType_UTCTimeOnly          = 0x8000,
   FIXFieldValueType_UTCDateOnly          = 0x9000,
   FIXFieldValueType_LocalMktDate         = 0xA000,
   FIXFieldValueType_TZTimeOnly           = 0xB000,
   FIXFieldValueType_TZTimestamp          = 0xC000,
   FIXFieldValueType_Language             = 0xD000,
   FIXFieldValueType_XMLData              = 0x20000,
   FIXFieldValueType_Data                 = 0x10000
} FIXFieldValueTypeEnum;

/**
 * string to FIXFieldValueTypeEnum enum conversion
 * @param[in] type - string type value
 * @return see FIXFieldValueTypeEnum
 */
FIXFieldValueTypeEnum str2FIXFieldValueType(char const* type);


#define IS_STRING_TYPE(type) ((type & 0xF000) > 0)
#define IS_INT_TYPE(type)    ((type & 0x0F) > 0)
#define IS_FLOAT_TYPE(type)  ((type & 0xF0) > 0)
#define IS_CHAR_TYPE(type)   ((type & 0xF00) > 0)
#define IS_DATA_TYPE(type)   ((type & 0xF0000) > 0)

/**
 * FIX parser attributes. Determine memory usage stategy
 */
typedef struct FIXParserAttrs
{
   uint32_t pageSize;     ///< Page size in bytes. Default 4096
   uint32_t maxPageSize;  ///< Maximum page size. 0 - page can be grow depends of tag value size, pageSize - page can not grow. Default 0
   uint32_t numPages;     ///< Pages allocated at parser creation. Default 1000
   uint32_t maxPages;     ///< Maximum alocated pages. 0 - not bounded, numPages - only numPages pages can be allocates. Default 0
   uint32_t numGroups;    ///< Groups allocated at parser creation. Default 1000
   uint32_t maxGroups;    ///< Maximum allocated groups. 0 - not bounded, numGroups - onlu numGroups groups can be allocated. Default 0
} FIXParserAttrs;

#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_TYPES_H */
