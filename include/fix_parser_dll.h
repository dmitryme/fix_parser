/**
 * @file   fix_parser_dll.h
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 12/22/2012 6:20:19 PM
 */

#ifdef _WINDLL
#   ifdef WIN32
#      ifdef fix_parser_EXPORTS
#         define FIX_PARSER_API __declspec(dllexport)
#      else
#         define FIX_PARSER_API __declspec(dllimport)
#      endif
#   endif
#else
# define FIX_PARSER_API
#endif
