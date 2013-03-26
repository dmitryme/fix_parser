/**
 * @file   fix_parser.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 07/30/2012 10:54:30 AM
 */

#ifndef FIX_PARSER_FIX_PARSER_H
#define FIX_PARSER_FIX_PARSER_H

#include "fix_types.h"
#include "fix_parser_dll.h"
#include "fix_msg.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * create new parser instance
 * @param[in] protFile - path to xml file with protocol description. see fix_parser/fix_descr directory for various FIX
 * protocol description
 * @param[in] attrs - parser attributes
 * @param[in] flags - parser flags. See PARSER_FLAG_CHECK_* values
 * @param[out] error - error description, if any. If error is returned, it must be destroyed by free(error)
 * @return new instance of FIX parser. if NULL, invoke fix_error_get_code(error), fix_error_get_text(error) for error description
 */
FIX_PARSER_API FIXParser* fix_parser_create(char const* protFile, FIXParserAttrs const* attrs, int32_t flags, FIXError** error);

/**
 * free parser instance.
 * @param[in] parser - pointer to parser instance
 */
FIX_PARSER_API void fix_parser_free(FIXParser* parser);

/**
 * return FIX protocol verision of the parser instance
 * @param[in] parser - pointer to parser instance
 * return FIX protocol version, NULL - in case of error
 */
FIX_PARSER_API char const* fix_parser_get_protocol_ver(FIXParser* parser);

/**
 * parse FIX encoded message
 * @param[in] parser - instance of FIX parser
 * @param[in] data - pointer to data win FIX message
 * @param[in] len - length of parsed data
 * @param[in] delimiter - FIX SOH
 * @param[in] stop - pointer to position in data, where parsing is stopped
 * @param[out] error - error descritption
 * @return new instance of parsed message
 */
FIX_PARSER_API FIXMsg* fix_parser_str_to_msg(FIXParser* parser, char const* data, uint32_t len, char delimiter, char const** stop, FIXError** error);

/**
 * pre-parse string and return pair SenderCompID and TargetCompID
 * @param[in] data - message to pre-parser
 * @param[in] len  - length of pre-parsed data
 * @param[in] delimiter - FIX SOH
 * @param[out] beginString - pointer to BeginString data
 * @param[out] beginStringLen - length of BeginString data
 * @param[out] msgType - pointer to MsgType data
 * @param[out] msgTypeLen - length of MsgType data
 * @param[out] senderCompID - pointer to SenderCompID data
 * @param[out] senderCompIDLen - length of senderCompID data
 * @param[out] targetCompID - pointer to TargetCompID data
 * @param[out] targetCompIDLen - length of  TargetCompID data
 * @param[out] msgSeqNum - message sequence number
 * @param[out] error - error description, if any. If error is returned it must be destroyed by fix_error_free(error)
 * @return FIX_SUCCESS - ok, FIX_FAILED - bad
 */
FIX_PARSER_API FIXErrCode fix_parser_get_header(char const* data, uint32_t len, char delimiter,
      char const** beginString, uint32_t* beginStringLen,
      char const** msgType, uint32_t *msgTypeLen,
      char const** senderCompID, uint32_t* senderCompIDLen,
      char const** targetCompID, uint32_t* targetCompIDLen,
      int64_t* msgSeqNum, FIXError** error);

#ifdef __cplusplus
}
#endif

#endif /* FIX_PARSER_FIX_PARSER_H */
