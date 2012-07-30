/// @file   fix_tag.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/25/2012 03:35:40 PM

#include <stdint.h>

#define TABLE_SIZE 64

enum FIXTagType
{
   FIXTagValue = 1,
   FIXTagGroup = 2
};

typedef struct FIXTagTable_ FIXTagTable;

typedef struct FIXTag_
{
   uint32_t num;
   enum FIXTagType type;
   union
   {
      struct
      {
         unsigned char* data;
         uint32_t len;
      } value;
      struct
      {
         uint32_t size;
         FIXTagTable** grpTbl;
      } groups;
   };
   struct FIXTag_* next;
} FIXTag;

FIXTagTable* new_table();
void free_table(FIXTagTable* tbl);

FIXTag* set_fix_tag(FIXTagTable* tbl, uint32_t tagNum, unsigned char const* data, uint32_t len);
FIXTag* get_fix_tag(FIXTagTable* tbl, uint32_t tagNum);
int rm_fix_tag(FIXTagTable* tbl, uint32_t tagNum);

FIXTagTable* add_fix_group(FIXTagTable* tbl, uint32_t tagNum);
FIXTagTable* get_fix_group(FIXTagTable* tbl, uint32_t tagNum, uint32_t grpIdx);
int rm_fix_group(FIXTagTable* tbl, uint32_t tagNum, uint32_t grpIdx);