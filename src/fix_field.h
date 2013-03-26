/**
 * @file   fix_field.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/25/2012 03:35:40 PM
 */

#ifndef FIX_PARSER_FIX_FIELD_H
#define FIX_PARSER_FIX_FIELD_H

#include "fix_types.h"
#include "fix_msg_priv.h"

#include <stdint.h>
#include <stdlib.h>

#pragma pack(push, 1)
#pragma pack(1)

#ifdef __cplusplus
extern "C"
{
#endif

#define GROUP_SIZE 64

/**
 * FIX field
 */
struct FIXField_
{
   FIXFieldDescr const* descr; ///< FIX field description
   struct FIXField_* next;     ///< next FIX field with the same hash key
   uint32_t body_len;          ///< length of field, if it is converted to string
   uint32_t size;              ///< size of field data
   char* data;                 ///< field value. All values converted to string
};

/**
 * FIX field group
 */
struct FIXGroup_
{
   FIXField* fields[GROUP_SIZE]; ///< FIX field hash table
   FIXFieldDescr const* parent_fdescr; ///< description of FIX field, which defines number of entries on group
   struct FIXGroup_* next;   ///< next group in pool of unused groups. If this group is used next == NULL
};

/**
 * array of FIX groups
 */
typedef struct FIXGroups_
{
   FIXGroup* group[1]; ///< pointer to FIX group array
} FIXGroups;

/**
 * set FIX field value
 * @param[in] msg    - FIX message
 * @param[in] grp    - FIX group, if FIX field is a part of FIX group, else must be NULL
 * @param[in] descr  - FIX field description
 * @param[in] data   - FIX field value
 * @param[in] len    - value length
 * @param[out] error - error description
 * @return pointer to changed FIX field, NULL in case of error
 */
FIXField* fix_field_set(FIXMsg* msg, FIXGroup* grp, FIXFieldDescr const* descr, unsigned char const* data, uint32_t len, FIXError** error);

/**
 * return FIX field by tag number
 * @param[in] msg - FIX message with required field
 * @param[in] grp - FIX group, if required FIX field is a part of FIX group
 * @param[in] tag - FIX field tag num
 * @return required FIX field, NULL - in case of error
 */
FIXField* fix_field_get(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag);

/**
 * delete FIX field by tag number
 * @param[in] msg - FIX message, with deleted FIX field
 * @param[in] grp - FIX group with deleted FIX field. Can be NULL
 * @param[in] tag - FIX field tag num
 * @param[out] error - error description
 * @return FIX_SUCCESS - ok, FIX_FAILED - error
 */
FIXErrCode fix_field_del(FIXMsg* msg, FIXGroup* grp, FIXTagNum tag, FIXError** error);

/**
 * add new FIX group
 */
FIXGroup*  fix_group_add(FIXMsg* msg, FIXGroup* grp, FIXFieldDescr const* descr, FIXField** fld, FIXError** error);

/**
 * return FIX group bu zer-based index
 */
FIXGroup*  fix_group_get(FIXMsg* msg, FIXGroup* tbl, FIXTagNum tag, uint32_t grpIdx, FIXError** error);

/**
 * remove FIX group
 */
FIXErrCode fix_group_del(FIXMsg* msg, FIXGroup* tbl, FIXTagNum tag, uint32_t grpIdx, FIXError** error);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif /* FIX_PARSER_FIX_FIELD_H */
