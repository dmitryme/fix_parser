/// @file   fix_error.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/24/2012 06:16:10 PM

#define FIX_FAILED                     -1
#define FIX_SUCCESS                     0
#define FIX_ERROR_TAG_HAS_WRONG_TYPE    1
#define FIX_ERROR_TAG_NOT_FOUND         2
#define FIX_ERROR_GROUP_WRONG_INDEX     3
#define FIX_ERROR_XML_ATTR_NOT_FOUND    4
#define FIX_ERROR_XML_ATTR_WRONG_VALUE  5
#define FIX_ERROR_PROTOCOL_XML_LOAD_FAILED 6
#define FIX_ERROR_UNKNOWN_FIELD         7
#define FIX_ERROR_WRONG_PROTOCOL_VER    8
#define FIX_ERROR_DUPLICATE_FIELD_DESCR 9
#define FIX_ERROR_UNKNOWN_MSG          10
#define FIX_ERROR_LIBXML               11

typedef struct FIXError_
{
   int code;
   char* text;
} FIXError;

FIXError* get_fix_last_error();
void reset_fix_error();