/// @file   fix_parser.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/30/2012 10:54:30 AM

#ifndef FIX_PARSER_FIX_PARSER_H
#define FIX_PARSER_FIX_PARSER_H

#include "fix_message.h"

#include <stdint.h>

#define FIXPARSER_FLAGS_CHECKCRC 0x01
#define FIXPARSER_FLAGS_VALIDATE 0x02

int fix_protocol_init(char const* protFile);
int parse_fix(FIXMessage** msg, uint32_t flags, char const* data, uint32_t len);

#endif // FIX_PARSER_FIX_PARSER_H