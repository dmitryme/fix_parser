/**
 * @file   fix_protocol_descr.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/24/2012 06:36:13 PM
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

/**
 * description of FIX field type (entry in dictionary types)
 */
typedef struct FIXFieldType_
{
   FIXTagNum tag;                   ///< tag number
   FIXFieldValueTypeEnum valueType; ///< type of field (string, number, length, etc)
   char* name;                      ///< textual representation of field
   struct FIXFieldType_* next;      ///< next type in chain
} FIXFieldType;

/**
 * descrion of FIX field
 */
typedef struct FIXFieldDescr_
{
   FIXFieldType* type;                  ///< type of FIX field
   FIXFieldCategoryEnum category;       ///< category - value or group
   uint8_t flags;                       ///< only FIELD_FLAG_REQUIRED is used
   uint32_t group_count;                ///< count of field descriptions in group
   struct FIXFieldDescr_*  group;       ///< hash table with field descriptions
   struct FIXFieldDescr_** group_index; ///< all field descriptions indexed as array
   struct FIXFieldDescr_*  next;        ///< next field description in hash table
   struct FIXFieldDescr_*  dataLenField; ///< reference to field description. Not NULL if this field has valueType == Data.
} FIXFieldDescr;

/**
 * FIX message description
 */
typedef struct FIXMsgDescr_
{
   char* type;                   ///< type. E.g. "A", "AE", "D"
   char* name;                   ///< textual message name
   uint32_t field_count;         ///< count of field descriptions
   FIXFieldDescr* fields;        ///< hash table with fields
   FIXFieldDescr** field_index;  ///< all fields indexed as array
   struct FIXMsgDescr_* next;    ///< next description with the same hash key
} FIXMsgDescr;

/**
 * FIX protocol description
 */
typedef struct FIXProtocolDescr_
{
   char* version;                                        ///< protocol version ("FIX.4.4", "FIX.5.0", etc)
   char* transportVersion;                               ///< version of transport protocol. If protocol doesn't have a transport transportVersion == version
   FIXFieldType* field_types[FIELD_TYPE_CNT];            ///< array of field types
   FIXFieldType* transport_field_types[FIELD_TYPE_CNT];  ///< field types of transport protocol
   FIXMsgDescr* messages[MSG_CNT];                       ///< message descriptions (transport and application levels)
} FIXProtocolDescr;

/**
 * parse protocol xml file and create protocol description
 * @param[out] error - in case of parse error, this error is set
 * @param[in] file - protocol xml file
 */
FIXProtocolDescr* fix_protocol_descr_create(FIXError* error, char const* file);

/**
 * destroy protocol description
 * @param[in] prot - protocol, which is being deleted
 */
void fix_protocol_descr_free(FIXProtocolDescr* prot);

/**
 * return field type by name
 * @param[in] ftypes - array of field types
 * @param[in] name - field type name
 * @return field type, NULL - not found
 */
FIXFieldType* fix_protocol_get_field_type(FIXFieldType* (*ftypes)[FIELD_TYPE_CNT], char const* name);

/**
 * get FIX message description by type
 * @param[in] parser - only used for setting parser error
 * @param[in] type - FIX message type
 * @return FIX message description, NULL - error
 */
FIXMsgDescr* fix_protocol_get_msg_descr(FIXParser* parser, char const* type);

/**
 * get FIX field description by tag number
 * @param[out] error - return an error, if any
 * @param[in] msg - FIX message description
 * @param[in] tag - FIX field tag
 * @return field description. NULL - error
 */
FIXFieldDescr* fix_protocol_get_field_descr(FIXError* error, FIXMsgDescr const* msg, FIXTagNum tag);

/**
 * get FIX field description from FIX grou description
 * @param[out] error - retur error if any
 * @param[in] field - FIX field description (group)
 * @param[in] tag - required FIX field tag number
 * @return FIX field description, NULL - error
 */
FIXFieldDescr* fix_protocol_get_group_descr(FIXError* error, FIXFieldDescr const* field, FIXTagNum tag);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif /* FIX_PARSER_FIX_PROTOCOL_DESCR_H */
