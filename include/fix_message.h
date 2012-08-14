/* @file   fix_message.h
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 07/30/2012 06:28:42 PM
*/

#ifndef FIX_PARSER_FIX_MESSAGE_H
#define FIX_PARSER_FIX_MESSAGE_H

#include "fix_types.h"

#include <stdint.h>

FIXMessage* new_fix_message(FIXParser* parser, FIXProtocolVerEnum ver, char const* msgType);
void free_fix_message(FIXMessage* msg);

FIXTag* get_tag(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum);
int del_tag(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum);

/*
FIXGroup* add_group(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum);
FIXGroup* get_group(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx);
int del_group(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, uint32_t grpIdx);
*/

FIXTag* set_tag_string(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, char const* val);
FIXTag* set_tag_long(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, long val);
FIXTag* set_tag_ulong(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, unsigned long val);
FIXTag* set_tag_char(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, char val);
FIXTag* set_tag_float(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, float val);

int get_tag_long(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, long* val);
int get_tag_ulong(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, unsigned long* val);
int get_tag_float(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, float* val);
int get_tag_char(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, char* val);
int get_tag_string(FIXMessage* msg, FIXGroup* grp, uint32_t tagNum, char* val, uint32_t len);

int fix_message_to_string(FIXMessage* msg, char delimiter, char* buff, uint32_t buffLen);

#endif /* FIX_PARSER_FIX_MESSAGE_H */
