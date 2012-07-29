/// @file   tag.h
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 07/25/2012 03:35:40 PM

#include <stdint.h>

#define TABLE_SIZE 64

enum TagType
{
   TagValue = 1,
   TagGroup = 2
};

typedef struct TagTable_ TagTable;

typedef struct Tag_
{
   uint32_t num;
   enum TagType type;
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
         TagTable** grpTbl;
      } groups;
   };
   struct Tag_* next;
} Tag;

TagTable* new_table();
void free_table(TagTable* tbl);

Tag* set_tag(TagTable* tbl, uint32_t tagNum, unsigned char const* data, uint32_t len);
Tag* get_tag(TagTable* tbl, uint32_t tagNum);
int rm_tag(TagTable* tbl, uint32_t tagNum);

TagTable* add_group(TagTable* tbl, uint32_t tagNum);
TagTable* get_grp(TagTable* tbl, uint32_t tagNum, uint32_t grpIdx);
int rm_grp(TagTable* tbl, uint32_t tagNum, uint32_t grpIdx);