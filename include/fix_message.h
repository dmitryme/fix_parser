/// @file   fix_message.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/30/2012 06:28:42 PM

#ifndef FIX_PARSER_FIX_MESSAGE_H
#define FIX_PARSER_FIX_MESSAGE_H

#include "fix_types.h"

#include <stdint.h>

#define FIX_MESSAGE_FLAGS_VALIDATE 0x01

FIXMessage* new_fix_message(FIXProtocolVerEnum ver, char const* msgType, uint32_t memSize, uint32_t flags);
void free_fix_message(FIXMessage* msg);

FIXTag* set_tag(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned char const* data, uint32_t len);
FIXTag* set_tag_fmt(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char const* fmt, ...);
FIXTag* get_tag(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum);
int del_tag(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum);

FIXTagTable* add_group(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum);
FIXTagTable* get_group(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, uint32_t grpIdx);
int del_group(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, uint32_t grpIdx);

FIXTag* set_tag_string(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char const* val);
FIXTag* set_tag_long(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, long val);
FIXTag* set_tag_ulong(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned long val);
FIXTag* set_tag_char(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char val);
FIXTag* set_tag_float(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, float val);

int get_tag_long(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, long* val);
int get_tag_ulong(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, unsigned long* val);
int get_tag_float(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, float* val);
int get_tag_char(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char* val);
int get_tag_string(FIXMessage* msg, FIXTagTable* grp, uint32_t tagNum, char* val, uint32_t len);

#endif // FIX_PARSER_FIX_MESSAGE_H