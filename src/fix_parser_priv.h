/**
 * @file   fix_parser_priv.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 10:54:30 AM
 */

#ifndef FIX_PARSER_FIX_PARSER_PRIV_H
#define FIX_PARSER_FIX_PARSER_PRIV_H

#include "fix_types.h"
#include "fix_protocol_descr.h"
#include "fix_page.h"
#include "fix_field.h"
#include "fix_error_priv.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * FIX parser data
 */
struct FIXParser_
{
   FIXProtocolDescr const* protocol;   ///< FIX protocol
   FIXParserAttrs attrs;               ///< attributes
   int32_t  flags;                     ///< flags
   FIXPage* page;                      ///< list of allocated memory pages
   uint32_t used_pages;                ///< count of memory pages in use
   FIXGroup* group;                    ///< allocated FIX groups
   uint32_t used_groups;               ///< count of used groups
};

/**
 * allocate new page by parser
 * @param[in] parser   - FIX parser
 * @param[in] pageSize - size of page
 * @param[out] error - error description
 * @return allocated page, NULL - if allocated page exceeded parser attributes
 */
FIXPage* fix_parser_alloc_page(FIXParser* parser, uint32_t pageSize, FIXError** error);

/**
 * free allocated page
 * @param[in] parser - page holder
 * @param[in] page - deallocated page
 * @return next used page or NULL if no more pages are used
 */
FIXPage* fix_parser_free_page(FIXParser* parser, FIXPage* page);

/**
 * allocate new FIX group
 * @param[in] parser - group allocator
 * @param[out] error - error description
 * @return allocated group pr NULL if parser attributes are exceeded
 */
FIXGroup* fix_parser_alloc_group(FIXParser* parser, FIXError** error);

/**
 * free allocated group
 * @param[in] parser - group holder
 * @param[in] group - deallocated group
 * @return next used group or NULL
 */
FIXGroup* fix_parser_free_group(FIXParser* parser, FIXGroup* group);

/**
 * parse string with mandatory fields. Mandatory fields are BeginString, BodyLength, CheckSum, MsgType
 * @param[in] data - string to parse
 * @param[in] len - length of data
 * @param[in] delimiter - FIX field SOH
 * @param[out] dbegin - points to begin of field value
 * @param[out] dend - points to end of field value
 * @param[out] error - error description
 * @return FIX field tag number or FIX_FAILED
 */
FIXTagNum fix_parser_parse_mandatory_field(
      char const* data, uint32_t len, char delimiter, char const** dbegin, char const** dend, FIXError** error);

/**
 * parser string with FIX field
 * @param[in] parser - FIX parser
 * @param[in] msg - FIX message, which will hold parsed field
 * @param[in] group - FIX group, which will hold parsed field. Can be NULL
 * @param[in] data - string to parser
 * @param[in] len - length of data
 * @param[in] delimiter - FIX field SOH
 * @param[out] fdescr - description of field
 * @param[out] dbegin - points to begin of field value
 * @param[out] dend - points to end of field value
 * @param[out] error - error description
 * @return FIX field tag number or FIX_FAILED
 */
FIXTagNum fix_parser_parse_field(
      FIXParser* parser, FIXMsg* msg, FIXGroup* group, char const* data, uint32_t len, char delimiter, FIXFieldDescr const** fdescr,
      char const** dbegin, char const** dend, FIXError** error);

/**
 * validate parser attributes
 * @param[in] attrs - attributes to validate
 * @param[out] error - error description
 * @return FIX_SUCCESS - ok, FIX_FAILED - invalid attributes, discover error for details
 */
FIXErrCode fix_parser_validate_attrs(FIXParserAttrs* attrs, FIXError** error);

/**
 * extract field value from string
 * @param[in] dbegin - begin of data
 * @param[in] len - length of data
 * @param[in] delimiter - FIX field SOH
 * @param[out] dend - end of field data
 * @param[out] error - error description, if any
 * @return FIX_SUCCESS - ok, FIX_FAILED - error
 */
FIXErrCode fix_parser_get_value(char const* dbegin, uint32_t len, char delimiter, char const** dend, FIXError* error);

/**
 * test field value
 * @param[in] fdescr - FIX field description
 * @param[in] dbegin - begin of value to validate
 * @param[in] dend - end of value to validate
 * @param[in] delimiter - FIX field SOH
 * @param[out] error - error description
 * @return FIX_SUCCESS - ok, FIX_FAILED - error
 */
FIXErrCode fix_parser_check_value(FIXFieldDescr const* fdescr, char const* dbegin, char const* dend, char delimiter, FIXError** error);

/**
 * parse string with group
 * @param[in] parser - FIX parser
 * @param[in] msg - FIX message, which will hold parsed group
 * @param[in] parentGroup - FIX group, which will hold nested group
 * @param[in] gdescr - FIX group description
 * @param[in] data - string to parser
 * @param[in] len - length of data
 * @param[out] stop - parse stop pointer
 * @param[out] error - error description
 * @return FIX_SUCCESS - ok, FIX_FAILED - error
 */
FIXErrCode fix_parser_parse_group(FIXParser* parser, FIXMsg* msg, FIXGroup* parentGroup, FIXFieldDescr const* gdescr, int64_t numGroups,
      char const* data, uint32_t len, char delimiter, char const** stop, FIXError** error);

#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_PARSER_PRIV_H */
