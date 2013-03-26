/**
 * @file   fix_field_tests.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 08/03/2012 02:41:02 PM
 */

#include  <fix_parser.h>
#include  <fix_parser_priv.h>
#include  <fix_msg_priv.h>
#include  <fix_field.h>
#include  <fix_types.h>
#include  <fix_protocol_descr.h>

#include <gtest/gtest.h>
#include <stdlib.h>

FIXMsg* new_fake_message(FIXParser* parser)
{
   FIXError* error = NULL;
   FIXMsg* msg = (FIXMsg*)calloc(1, sizeof(FIXMsg));
   msg->fields = fix_parser_alloc_group(parser, &error);
   msg->parser = parser;
   msg->pages = msg->curr_page = fix_parser_alloc_page(parser, 0, &error);
   return msg;
}

FIXFieldDescr* new_fdescr(int tag, FIXFieldCategoryEnum category, FIXFieldValueTypeEnum valueType)
{
   // it will be leaks, but who cares...
   FIXFieldType* type = (FIXFieldType*)calloc(sizeof(FIXFieldType), 1);
   type->tag = tag;
   type->valueType = valueType;
   FIXFieldDescr* fdescr = (FIXFieldDescr*)calloc(sizeof(FIXFieldDescr), 1);
   fdescr->type = type;
   fdescr->category = category;
   return fdescr;
}

TEST(FixFieldTests, SetTagTest)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);

   FIXMsg* msg = new_fake_message(parser);

   char const val[] = {"1000"};
   FIXField* field = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)val, strlen(val), &error);
   ASSERT_EQ(msg->fields->fields[1], field);
   ASSERT_TRUE(field->descr != NULL);
   ASSERT_EQ(field->descr->type->tag, 1);
   ASSERT_EQ(field->descr->category, FIXFieldCategory_Value);
   ASSERT_TRUE(field->next == NULL);
   ASSERT_TRUE(!strncmp((char const*)field->data, val, strlen(val)));
   ASSERT_EQ(field->size, strlen(val));

   ASSERT_EQ(msg->curr_page->size, 512U);
   ASSERT_EQ(msg->curr_page->offset, 4 + sizeof(FIXField) + 4 + strlen(val));
   ASSERT_TRUE(msg->curr_page->next == NULL);

   FIXField* field11 = fix_field_set(msg, NULL, new_fdescr(2, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)val, strlen(val), &error);
   ASSERT_EQ(msg->fields->fields[2], field11);
   ASSERT_EQ(field11->descr->type->tag, 2);
   ASSERT_EQ(field11->descr->category, FIXFieldCategory_Value);
   ASSERT_TRUE(field11->next == NULL);
   ASSERT_TRUE(!strncmp((char const*)field11->data, val, strlen(val)));
   ASSERT_EQ(field11->size, strlen(val));

   FIXField* field12 = fix_field_set(msg, NULL, new_fdescr(30, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)val, strlen(val), &error);
   ASSERT_EQ(msg->fields->fields[30], field12);
   ASSERT_EQ(field12->descr->type->tag, 30);
   ASSERT_EQ(field12->descr->category, FIXFieldCategory_Value);
   ASSERT_TRUE(field12->next == NULL);
   ASSERT_TRUE(!strncmp((char const*)field12->data, val, strlen(val)));
   ASSERT_EQ(field12->size, strlen(val));

   char const val1[] = {"2000"};
   FIXField* field1 = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)val1, strlen(val1), &error);
   ASSERT_EQ(field, field1);
   ASSERT_EQ(field1->descr->type->tag, 1);
   ASSERT_EQ(field1->descr->category, FIXFieldCategory_Value);
   ASSERT_TRUE(field1->next == NULL);
   ASSERT_TRUE(!strncmp((char const*)field1->data, val1, strlen(val1)));
   ASSERT_EQ(field1->size, strlen(val1));

   ASSERT_EQ(msg->curr_page->size, 512U);
   ASSERT_EQ(msg->curr_page->offset, 3 * (4 + sizeof(FIXField)) + 4 + strlen(val1) + 4 + strlen(val) + 4 + strlen(val));
   ASSERT_TRUE(msg->curr_page->next == NULL);

   char const val2[] = {"64"};
   FIXField* field2 = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)val2, strlen(val2), &error);
   ASSERT_EQ(field2, field);
   ASSERT_EQ(field2->descr->type->tag, 1);
   ASSERT_EQ(field2->descr->category, FIXFieldCategory_Value);
   ASSERT_TRUE(field2->next == NULL);
   ASSERT_TRUE(!strncmp((char const*)field2->data, val2, strlen(val2)));
   ASSERT_EQ(field1->size, strlen(val2));

   ASSERT_EQ(msg->curr_page->size, 512U);
   ASSERT_EQ(msg->curr_page->offset, 3 * (4 + sizeof(FIXField)) + 4 + strlen(val1) + 4 + strlen(val) + 4 + strlen(val));
   ASSERT_TRUE(msg->curr_page->next == NULL);

   char const txt[] = "Hello world!";
   FIXField* field3 = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)txt, strlen(txt), &error);
   ASSERT_EQ(field3, field);
   ASSERT_EQ(field3->descr->type->tag, 1);
   ASSERT_EQ(field3->descr->category, FIXFieldCategory_Value);
   ASSERT_TRUE(field3->next == NULL);
   ASSERT_TRUE(!strncmp((char const*)field3->data, txt, strlen(txt)));
   ASSERT_EQ(field3->size, strlen(txt));

   ASSERT_EQ(msg->curr_page->size, 512U);
   ASSERT_EQ(msg->curr_page->offset, 3 * (4 + sizeof(FIXField)) + 4 + strlen(val1) + 4 + strlen(val) + 4 + strlen(val) + 4 + strlen(txt));
   ASSERT_TRUE(msg->curr_page->next == NULL);

   fix_parser_free(parser);
   free(msg);
}

TEST(FixFieldTests, DelTagTest)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);

   FIXMsg* msg = new_fake_message(parser);

   int val = 1000;
   FIXField* field = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_Int),
         (unsigned char const*)&val, sizeof(val), &error);
   ASSERT_EQ(msg->fields->fields[1], field);
   ASSERT_EQ(field->descr->type->tag, 1);
   ASSERT_EQ(field->descr->category, FIXFieldCategory_Value);
   ASSERT_TRUE(field->next == NULL);
   ASSERT_EQ(*(long*)field->data, val);

   ASSERT_EQ(msg->curr_page->size, 512U);
   ASSERT_EQ(msg->curr_page->offset, 4 + sizeof(FIXField) + 4 + sizeof(val));
   ASSERT_TRUE(msg->curr_page->next == NULL);

   uint32_t val1 = 2000;
   FIXField* field1 = fix_field_set(msg, NULL, new_fdescr(65, FIXFieldCategory_Value, FIXFieldValueType_Int),
         (unsigned char const*)&val1, sizeof(val1), &error);
   ASSERT_EQ(msg->fields->fields[1], field1);
   ASSERT_EQ(field1->descr->type->tag, 65);
   ASSERT_EQ(field1->descr->category, FIXFieldCategory_Value);
   ASSERT_EQ(field1->next, field);
   ASSERT_EQ(*(uint64_t*)field1->data, val1);

   ASSERT_EQ(msg->curr_page->size, 512U);
   ASSERT_EQ(msg->curr_page->offset, 2 * (4 + sizeof(FIXField)) + 4 + sizeof(val) + 4 + sizeof(val1));
   ASSERT_TRUE(msg->curr_page->next == NULL);

   uint32_t val2 = 3000;
   FIXField* field2 = fix_field_set(msg, NULL, new_fdescr(129, FIXFieldCategory_Value, FIXFieldValueType_Int),
         (unsigned char const*)&val2, sizeof(val2), &error);
   ASSERT_EQ(msg->fields->fields[1], field2);
   ASSERT_EQ(field2->descr->type->tag, 129);
   ASSERT_EQ(field2->descr->category, FIXFieldCategory_Value);
   ASSERT_EQ(field2->next, field1);
   ASSERT_EQ(*(uint64_t*)field2->data, val2);

   ASSERT_EQ(msg->curr_page->size, 512U);
   ASSERT_EQ(msg->curr_page->offset, 3 * (4 + sizeof(FIXField)) + 4 + sizeof(val) + 4 + sizeof(val1) + 4 + sizeof(val));
   ASSERT_TRUE(msg->curr_page->next == NULL);

   int val3 = 4000;
   FIXField* field3 = fix_field_set(msg, NULL, new_fdescr(193, FIXFieldCategory_Value, FIXFieldValueType_Int),
         (unsigned char const*)&val3, sizeof(val3), &error);

   ASSERT_EQ(msg->fields->fields[1], field3);
   ASSERT_EQ(field3->descr->type->tag, 193);
   ASSERT_EQ(field3->descr->category, FIXFieldCategory_Value);
   ASSERT_EQ(field3->next, field2);
   ASSERT_EQ(*(long*)field3->data, val3);

   ASSERT_EQ(msg->curr_page->size, 512U);
   ASSERT_EQ(msg->curr_page->offset, 4 * (4 + sizeof(FIXField)) + 4 + sizeof(val) + 4 + sizeof(val) + 4 + sizeof(val1) + 4 + sizeof(val3));
   ASSERT_TRUE(msg->curr_page->next == NULL);

   int res = fix_field_del(msg, NULL, 1, &error);
   ASSERT_EQ(res, FIX_SUCCESS);
   ASSERT_EQ(msg->fields->fields[1], field3);
   ASSERT_EQ(msg->fields->fields[1]->next, field2);
   ASSERT_EQ(msg->fields->fields[1]->next->next, field1);
   ASSERT_TRUE(msg->fields->fields[1]->next->next->next == NULL);

   ASSERT_EQ(msg->curr_page->size, 512U);
   ASSERT_EQ(msg->curr_page->offset, 4 * (4 + sizeof(FIXField)) + 4 + sizeof(val) + 4 + sizeof(val) + 4 + sizeof(val1) + 4 + sizeof(val3));
   ASSERT_TRUE(msg->curr_page->next == NULL);

   res = fix_field_del(msg, msg->fields, 129, &error);
   ASSERT_EQ(res, FIX_SUCCESS);
   ASSERT_EQ(msg->fields->fields[1], field3);
   ASSERT_EQ(msg->fields->fields[1]->next, field1);
   ASSERT_TRUE(msg->fields->fields[1]->next->next == NULL);

   res = fix_field_del(msg, msg->fields, 193, &error);
   ASSERT_EQ(res, FIX_SUCCESS);
   ASSERT_EQ(msg->fields->fields[1], field1);
   ASSERT_TRUE(msg->fields->fields[1]->next == NULL);

   res = fix_field_del(msg, msg->fields, 65, &error);
   ASSERT_EQ(res, FIX_SUCCESS);
   ASSERT_TRUE(msg->fields->fields[1] == NULL);

   fix_parser_free(parser);
   free(msg);
}

TEST(FixFieldTests, GetTagTest)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);

   FIXMsg* msg = new_fake_message(parser);

   long val = 1000;
   FIXField* field = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)&val, sizeof(val), &error);
   ASSERT_EQ(msg->fields->fields[1], field);
   ASSERT_EQ(field->descr->type->tag, 1);
   ASSERT_EQ(field->descr->category, FIXFieldCategory_Value);
   ASSERT_TRUE(field->next == NULL);
   ASSERT_EQ(*(long*)field->data, val);

   long val1 = 2000;
   FIXField* field1 = fix_field_set(msg, NULL, new_fdescr(65, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)&val1, sizeof(val1), &error);
   ASSERT_EQ(msg->fields->fields[1], field1);
   ASSERT_EQ(msg->fields->fields[1]->next, field);
   ASSERT_EQ(field1->descr->type->tag, 65);
   ASSERT_EQ(field1->descr->category, FIXFieldCategory_Value);
   ASSERT_EQ(field1->next, field);
   ASSERT_EQ(*(long*)field1->data, val1);

   long val2 = 3000;
   FIXField* field2 = fix_field_set(msg, NULL, new_fdescr(129, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)&val2, sizeof(val2), &error);
   ASSERT_EQ(msg->fields->fields[1], field2);
   ASSERT_EQ(msg->fields->fields[1]->next, field1);
   ASSERT_EQ(msg->fields->fields[1]->next->next, field);
   ASSERT_EQ(field2->descr->type->tag, 129);
   ASSERT_EQ(field2->descr->category, FIXFieldCategory_Value);
   ASSERT_EQ(field2->next, field1);
   ASSERT_EQ(*(long*)field2->data, val2);

   uint64_t val3 = 4000;
   FIXField* field3 = fix_field_set(msg, NULL, new_fdescr(193, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)&val3, sizeof(val3), &error);
   ASSERT_EQ(msg->fields->fields[1], field3);
   ASSERT_EQ(msg->fields->fields[1]->next, field2);
   ASSERT_EQ(msg->fields->fields[1]->next->next, field1);
   ASSERT_EQ(msg->fields->fields[1]->next->next->next, field);
   ASSERT_EQ(field3->descr->type->tag, 193);
   ASSERT_EQ(field3->descr->category, FIXFieldCategory_Value);
   ASSERT_EQ(field3->next, field2);
   ASSERT_EQ(*(uint64_t*)field3->data, val3);

   long val4 = 4000;
   FIXField* field4 = fix_field_set(msg, NULL, new_fdescr(2, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char const*)&val4, sizeof(val4), &error);
   ASSERT_EQ(msg->fields->fields[1], field3);
   ASSERT_EQ(msg->fields->fields[1]->next, field2);
   ASSERT_EQ(msg->fields->fields[1]->next->next, field1);
   ASSERT_EQ(msg->fields->fields[1]->next->next->next, field);
   ASSERT_EQ(msg->fields->fields[2], field4);
   ASSERT_EQ(field4->descr->type->tag, 2);
   ASSERT_EQ(field4->descr->category, FIXFieldCategory_Value);
   ASSERT_TRUE(field4->next == NULL);
   ASSERT_EQ(*(long*)field4->data, val4);

   ASSERT_EQ(fix_field_get(msg, msg->fields, 1), field);
   ASSERT_EQ(fix_field_get(msg, msg->fields, 129), field2);
   ASSERT_EQ(fix_field_get(msg, msg->fields, 193), field3);
   ASSERT_EQ(fix_field_get(msg, msg->fields, 2), field4);

   ASSERT_TRUE(fix_field_get(msg, msg->fields, 3) == NULL);

   fix_parser_free(parser);
   free(msg);
}

TEST(FixFieldTests, AddGetDelGroupTest)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);

   FIXMsg* msg = new_fake_message(parser);

   FIXField* field = NULL;
   FIXGroup* grp = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field, &error);
   ASSERT_TRUE(field != NULL);
   ASSERT_TRUE(grp != NULL);
   ASSERT_EQ(field->size, 1U);
   ASSERT_EQ(msg->used_groups, grp);
   ASSERT_EQ(parser->used_groups, 2U);

   long val = 100;
   FIXField* field1 = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String),
         (unsigned char*)&val, sizeof(val), &error);
   ASSERT_TRUE(field1 == NULL);
   ASSERT_TRUE(error != NULL);
   ASSERT_EQ(error->code, FIX_ERROR_FIELD_HAS_WRONG_TYPE);

   FIXField* field11 = NULL;
   FIXGroup* grp1 = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field11, &error);
   ASSERT_TRUE(field11 != NULL);
   ASSERT_TRUE(grp1 != NULL);
   ASSERT_EQ(field11->size, 2U);
   ASSERT_EQ(msg->used_groups, grp1);
   ASSERT_EQ(msg->used_groups->next, grp);
   ASSERT_EQ(parser->used_groups, 3U);
   ASSERT_TRUE(parser->group == NULL);

   FIXField* field2 = NULL;
   FIXGroup* grp2 = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field2, &error);
   ASSERT_TRUE(field2 != NULL);
   ASSERT_TRUE(grp2 != NULL);
   ASSERT_EQ(field2->size, 3U);
   ASSERT_EQ(msg->used_groups, grp2);
   ASSERT_EQ(msg->used_groups->next, grp1);
   ASSERT_EQ(msg->used_groups->next->next, grp);
   ASSERT_EQ(parser->used_groups, 4U);
   ASSERT_TRUE(parser->group == NULL);

   FIXField* field3 = NULL;
   FIXGroup* grp3 = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field3, &error);
   ASSERT_TRUE(grp3 != NULL);
   ASSERT_EQ(field3->size, 4U);
   ASSERT_EQ(msg->used_groups, grp3);
   ASSERT_EQ(msg->used_groups->next, grp2);
   ASSERT_EQ(msg->used_groups->next->next, grp1);
   ASSERT_EQ(msg->used_groups->next->next->next, grp);
   ASSERT_EQ(parser->used_groups, 5U);
   ASSERT_TRUE(parser->group == NULL);

   FIXGroup* grp4 = fix_group_get(msg, NULL, 1, 4, &error);
   ASSERT_TRUE(grp4 == NULL);
   ASSERT_TRUE(error != NULL);
   ASSERT_EQ(error->code, FIX_ERROR_GROUP_WRONG_INDEX);

   ASSERT_EQ(fix_group_get(msg, NULL, 1, 3, &error), grp3);
   ASSERT_EQ(fix_group_get(msg, NULL, 1, 2, &error), grp2);
   ASSERT_EQ(fix_group_get(msg, NULL, 1, 1, &error), grp1);
   ASSERT_EQ(fix_group_get(msg, NULL, 1, 0, &error), grp);

   FIXGroups* grps = (FIXGroups*)field->data;
   ASSERT_EQ(field->size, 4U);
   ASSERT_EQ(grps->group[0], grp);
   ASSERT_EQ(grps->group[1], grp1);
   ASSERT_EQ(grps->group[2], grp2);
   ASSERT_EQ(grps->group[3], grp3);

   ASSERT_TRUE(parser->group == NULL);
   ASSERT_EQ(msg->used_groups, grp3);

   ASSERT_EQ(fix_group_del(msg, NULL, 1, 3, &error), FIX_SUCCESS);
   ASSERT_EQ(parser->group, grp3);
   ASSERT_TRUE(parser->group->next == NULL);
   ASSERT_EQ(msg->used_groups, grp2);
   ASSERT_EQ(msg->used_groups->next, grp1);
   ASSERT_EQ(msg->used_groups->next->next, grp);
   ASSERT_TRUE(msg->used_groups->next->next->next == NULL);

   ASSERT_TRUE(field->size == 3);
   ASSERT_TRUE(grps->group[0] == grp);
   ASSERT_TRUE(grps->group[1] == grp1);
   ASSERT_TRUE(grps->group[2] == grp2);
   ASSERT_TRUE(grps->group[3] == NULL);

   ASSERT_EQ(fix_group_del(msg, NULL, 1, 1, &error), FIX_SUCCESS);
   ASSERT_EQ(parser->group, grp1);
   ASSERT_EQ(parser->group->next, grp3);
   ASSERT_TRUE(parser->group->next->next == NULL);
   ASSERT_EQ(msg->used_groups, grp2);
   ASSERT_EQ(msg->used_groups->next, grp);
   ASSERT_TRUE(msg->used_groups->next->next == NULL);

   ASSERT_EQ(field->size, 2U);
   ASSERT_EQ(grps->group[0], grp);
   ASSERT_EQ(grps->group[1], grp2);
   ASSERT_TRUE(grps->group[2] == NULL);
   ASSERT_TRUE(grps->group[3] == NULL);

   ASSERT_EQ(fix_group_del(msg, NULL, 1, 0, &error), FIX_SUCCESS);
   ASSERT_EQ(parser->group, grp);
   ASSERT_EQ(parser->group->next, grp1);
   ASSERT_EQ(parser->group->next->next, grp3);
   ASSERT_EQ(msg->used_groups, grp2);
   ASSERT_TRUE(msg->used_groups->next == NULL);

   ASSERT_TRUE(field->size == 1);
   ASSERT_EQ(grps->group[0], grp2);
   ASSERT_TRUE(grps->group[1] == NULL);
   ASSERT_TRUE(grps->group[2] == NULL);
   ASSERT_TRUE(grps->group[3] == NULL);

   ASSERT_EQ(fix_group_del(msg, NULL, 1, 0, &error), FIX_SUCCESS);
   ASSERT_TRUE(fix_field_get(msg, NULL, 1) == NULL);
   ASSERT_EQ(parser->group, grp2);
   ASSERT_EQ(parser->group->next, grp);
   ASSERT_EQ(parser->group->next->next, grp1);
   ASSERT_EQ(parser->group->next->next->next, grp3);
   ASSERT_TRUE(msg->used_groups == NULL);

   fix_parser_free(parser);
   free(msg);
}

TEST(FixFieldTests, NestedGroupsTest)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);

   FIXMsg* msg = new_fake_message(parser);

   FIXField* field = NULL;
   FIXGroup* grp = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field, &error);
   ASSERT_TRUE(field != NULL);
   ASSERT_TRUE(grp != NULL);
   ASSERT_EQ(field->size, 1U);
   ASSERT_EQ(msg->used_groups, grp);
   ASSERT_EQ(parser->used_groups, 2U);

   FIXField* nested_field = NULL;
   FIXGroup* nested_grp = fix_group_add(msg, grp, new_fdescr(65, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &nested_field, &error);
   ASSERT_TRUE(nested_field != NULL);
   ASSERT_TRUE(nested_grp != NULL);
   ASSERT_EQ(nested_field->descr->category, FIXFieldCategory_Group);
   ASSERT_EQ(parser->used_groups, 3U);

   ASSERT_EQ(msg->used_groups, nested_grp);
   ASSERT_EQ(msg->used_groups->next, grp);
   ASSERT_TRUE(msg->used_groups->next->next == NULL);

   FIXField* nested_field1 = NULL;
   FIXGroup* nested_grp1 = fix_group_add(msg, nested_grp, new_fdescr(131, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &nested_field1, &error);
   ASSERT_TRUE(nested_field1 != NULL);
   ASSERT_TRUE(nested_grp1 != NULL);
   ASSERT_EQ(nested_field1->descr->category, FIXFieldCategory_Group);
   ASSERT_EQ(parser->used_groups, 4U);

   ASSERT_EQ(msg->used_groups, nested_grp1);
   ASSERT_EQ(msg->used_groups->next, nested_grp);
   ASSERT_EQ(msg->used_groups->next->next, grp);
   ASSERT_TRUE(msg->used_groups->next->next->next == NULL);

   ASSERT_EQ(fix_group_del(msg, nested_grp, 131, 0, &error), FIX_SUCCESS);

   ASSERT_EQ(msg->used_groups, nested_grp);
   ASSERT_EQ(msg->used_groups->next, grp);
   ASSERT_TRUE(msg->used_groups->next->next == NULL);
   ASSERT_EQ(parser->group, nested_grp1);

   ASSERT_EQ(fix_group_del(msg, NULL, 1, 0, &error), FIX_SUCCESS);
   ASSERT_TRUE(msg->used_groups == NULL);
   ASSERT_EQ(parser->group, grp);
   ASSERT_EQ(parser->group->next, nested_grp);
   ASSERT_EQ(parser->group->next->next, nested_grp1);
   ASSERT_TRUE(parser->group->next->next->next == NULL);

   fix_parser_free(parser);
   free(msg);
}
