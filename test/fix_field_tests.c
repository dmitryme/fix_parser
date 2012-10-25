/* @file   fix_field_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/03/2012 02:41:02 PM
*/

#include  <fix_parser.h>
#include  <fix_parser_priv.h>
#include  <fix_msg_priv.h>
#include  <fix_field.h>
#include  <fix_types.h>
#include  <fix_protocol_descr.h>

#include <check.h>
#include <stdlib.h>

FIXMsg* new_fake_message(FIXParser* parser)
{
   FIXMsg* msg = calloc(1, sizeof(FIXMsg));
   msg->fields = fix_parser_alloc_group(parser);
   msg->parser = parser;
   msg->pages = msg->curr_page = fix_parser_alloc_page(parser, 0);
   return msg;
}

FIXFieldDescr* new_fdescr(int tag, FIXFieldCategoryEnum category, FIXFieldValueTypeEnum valueType)
{
   // it will be leaks, but who cares...
   FIXFieldType* type = calloc(sizeof(FIXFieldType), 1);
   type->tag = tag;
   type->valueType = valueType;
   FIXFieldDescr* fdescr = calloc(sizeof(FIXFieldDescr), 1);
   fdescr->type = type;
   fdescr->category = category;
   return fdescr;
}

START_TEST(SetTagTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create(&attrs, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   char const val[] = {"1000"};
   FIXField* field = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)val, strlen(val));
   fail_unless(msg->fields->fields[1] == field);
   fail_unless(field->descr != NULL);
   fail_unless(field->descr->type->tag == 1);
   fail_unless(field->descr->category == FIXFieldCategory_Value);
   fail_unless(field->next == NULL);
   fail_unless(!strncmp(field->data, val, strlen(val)));
   fail_unless(field->size == strlen(val));

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 + sizeof(FIXField) + 4 + strlen(val));
   fail_unless(msg->curr_page->next == NULL);

   FIXField* field11 = fix_field_set(msg, NULL, new_fdescr(2, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)val, strlen(val));
   fail_unless(msg->fields->fields[2] == field11);
   fail_unless(field11->descr->type->tag == 2);
   fail_unless(field11->descr->category == FIXFieldCategory_Value);
   fail_unless(field11->next == NULL);
   fail_unless(!strncmp(field11->data, val, strlen(val)));
   fail_unless(field11->size == strlen(val));

   FIXField* field12 = fix_field_set(msg, NULL, new_fdescr(30, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)val, strlen(val));
   fail_unless(msg->fields->fields[30] == field12);
   fail_unless(field12->descr->type->tag == 30);
   fail_unless(field12->descr->category == FIXFieldCategory_Value);
   fail_unless(field12->next == NULL);
   fail_unless(!strncmp(field12->data, val, strlen(val)));
   fail_unless(field12->size == strlen(val));

   char const val1[] = {"2000"};
   FIXField* field1 = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)val1, strlen(val1));
   fail_unless(field == field1);
   fail_unless(field1->descr->type->tag == 1);
   fail_unless(field1->descr->category == FIXFieldCategory_Value);
   fail_unless(field1->next == NULL);
   fail_unless(!strncmp(field1->data, val1, strlen(val1)));
   fail_unless(field1->size == strlen(val1));

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 3 * (4 + sizeof(FIXField)) + 4 + strlen(val1) + 4 + strlen(val) + 4 + strlen(val));
   fail_unless(msg->curr_page->next == NULL);

   char const val2[] = {"64"};
   FIXField* field2 = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)val2, strlen(val2));
   fail_unless(field2 == field);
   fail_unless(field2->descr->type->tag == 1);
   fail_unless(field2->descr->category == FIXFieldCategory_Value);
   fail_unless(field2->next == NULL);
   fail_unless(!strncmp(field2->data, val2, strlen(val2)));
   fail_unless(field1->size == strlen(val2));

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 3 * (4 + sizeof(FIXField)) + 4 + strlen(val1) + 4 + strlen(val) + 4 + strlen(val));
   fail_unless(msg->curr_page->next == NULL);

   char const txt[] = "Hello world!";
   FIXField* field3 = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)txt, strlen(txt));
   fail_unless(field3 == field);
   fail_unless(field3->descr->type->tag == 1);
   fail_unless(field3->descr->category  == FIXFieldCategory_Value);
   fail_unless(field3->next == NULL);
   fail_unless(!strncmp(field3->data, txt, strlen(txt)));
   fail_unless(field3->size == strlen(txt));

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 3 * (4 + sizeof(FIXField)) + 4 + strlen(val1) + 4 + strlen(val) + 4 + strlen(val) + 4 + strlen(txt));
   fail_unless(msg->curr_page->next == NULL);

   fix_parser_free(parser);
   free(msg);
}
END_TEST

START_TEST(DelTagTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create(&attrs, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   long val = 1000;
   FIXField* field = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->fields->fields[1] == field);
   fail_unless(field->descr->type->tag == 1);
   fail_unless(field->descr->category == FIXFieldCategory_Value);
   fail_unless(field->next == NULL);
   fail_unless(*(long*)field->data == val);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 + sizeof(FIXField) + 4 + sizeof(val));
   fail_unless(msg->curr_page->next == NULL);

   long val1 = 2000;
   FIXField* field1 = fix_field_set(msg, NULL, new_fdescr(65, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)&val1, sizeof(val1));
   fail_unless(msg->fields->fields[1] == field1);
   fail_unless(field1->descr->type->tag == 65);
   fail_unless(field1->descr->category == FIXFieldCategory_Value);
   fail_unless(field1->next == field);
   fail_unless(*(long*)field1->data == val1);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 2 * (4 + sizeof(FIXField)) + 4 + sizeof(val) + 4 + sizeof(val1));
   fail_unless(msg->curr_page->next == NULL);

   long val2 = 3000;
   FIXField* field2 = fix_field_set(msg, NULL, new_fdescr(129, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)&val2, sizeof(val2));
   fail_unless(msg->fields->fields[1] == field2);
   fail_unless(field2->descr->type->tag == 129);
   fail_unless(field2->descr->category == FIXFieldCategory_Value);
   fail_unless(field2->next == field1);
   fail_unless(*(long*)field2->data == val2);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 3 * (4 + sizeof(FIXField)) + 4 + sizeof(val) + 4 + sizeof(val1) + 4 + sizeof(val));
   fail_unless(msg->curr_page->next == NULL);

   long val3 = 4000;
   FIXField* field3 = fix_field_set(msg, NULL, new_fdescr(193, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)&val3, sizeof(val3));

   fail_unless(msg->fields->fields[1] == field3);
   fail_unless(field3->descr->type->tag == 193);
   fail_unless(field3->descr->category == FIXFieldCategory_Value);
   fail_unless(field3->next == field2);
   fail_unless(*(long*)field3->data == val3);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 * (4 + sizeof(FIXField)) + 4 + sizeof(val) + 4 + sizeof(val) + 4 + sizeof(val1) + 4 + sizeof(val3));
   fail_unless(msg->curr_page->next == NULL);

   int res = fix_field_del(msg, NULL, 1);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->fields->fields[1] == field3);
   fail_unless(msg->fields->fields[1]->next == field2);
   fail_unless(msg->fields->fields[1]->next->next == field1);
   fail_unless(msg->fields->fields[1]->next->next->next == NULL);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 * (4 + sizeof(FIXField)) + 4 + sizeof(val) + 4 + sizeof(val) + 4 + sizeof(val1) + 4 + sizeof(val3));
   fail_unless(msg->curr_page->next == NULL);

   res = fix_field_del(msg, msg->fields, 129);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->fields->fields[1] == field3);
   fail_unless(msg->fields->fields[1]->next == field1);
   fail_unless(msg->fields->fields[1]->next->next == NULL);

   res = fix_field_del(msg, msg->fields, 193);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->fields->fields[1] == field1);
   fail_unless(msg->fields->fields[1]->next == NULL);

   res = fix_field_del(msg, msg->fields, 65);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->fields->fields[1] == NULL);

   fix_parser_free(parser);
   free(msg);
}
END_TEST

START_TEST(GetTagTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create(&attrs, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   long val = 1000;
   FIXField* field = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->fields->fields[1] == field);
   fail_unless(field->descr->type->tag == 1);
   fail_unless(field->descr->category == FIXFieldCategory_Value);
   fail_unless(field->next == NULL);
   fail_unless(*(long*)field->data, val);

   long val1 = 2000;
   FIXField* field1 = fix_field_set(msg, NULL, new_fdescr(65, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)&val1, sizeof(val1));
   fail_unless(msg->fields->fields[1] == field1);
   fail_unless(msg->fields->fields[1]->next == field);
   fail_unless(field1->descr->type->tag == 65);
   fail_unless(field1->descr->category == FIXFieldCategory_Value);
   fail_unless(field1->next == field);
   fail_unless(*(long*)field1->data, val1);

   long val2 = 3000;
   FIXField* field2 = fix_field_set(msg, NULL, new_fdescr(129, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)&val2, sizeof(val2));
   fail_unless(msg->fields->fields[1] == field2);
   fail_unless(msg->fields->fields[1]->next == field1);
   fail_unless(msg->fields->fields[1]->next->next == field);
   fail_unless(field2->descr->type->tag == 129);
   fail_unless(field2->descr->category == FIXFieldCategory_Value);
   fail_unless(field2->next == field1);
   fail_unless(*(long*)field2->data, val2);

   long val3 = 4000;
   FIXField* field3 = fix_field_set(msg, NULL, new_fdescr(193, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)&val3, sizeof(val3));
   fail_unless(msg->fields->fields[1] == field3);
   fail_unless(msg->fields->fields[1]->next == field2);
   fail_unless(msg->fields->fields[1]->next->next == field1);
   fail_unless(msg->fields->fields[1]->next->next->next == field);
   fail_unless(field3->descr->type->tag == 193);
   fail_unless(field3->descr->category == FIXFieldCategory_Value);
   fail_unless(field3->next == field2);
   fail_unless(*(long*)field2->data, val3);

   long val4 = 4000;
   FIXField* field4 = fix_field_set(msg, NULL, new_fdescr(2, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char const*)&val4, sizeof(val4));
   fail_unless(msg->fields->fields[1] == field3);
   fail_unless(msg->fields->fields[1]->next == field2);
   fail_unless(msg->fields->fields[1]->next->next == field1);
   fail_unless(msg->fields->fields[1]->next->next->next == field);
   fail_unless(msg->fields->fields[2] == field4);
   fail_unless(field4->descr->type->tag == 2);
   fail_unless(field4->descr->category == FIXFieldCategory_Value);
   fail_unless(field4->next == NULL);
   fail_unless(*(long*)field4->data, val4);

   fail_unless(fix_field_get(msg, msg->fields, 1) == field);
   fail_unless(fix_field_get(msg, msg->fields, 129) == field2);
   fail_unless(fix_field_get(msg, msg->fields, 193) == field3);
   fail_unless(fix_field_get(msg, msg->fields, 2) == field4);

   fail_unless(fix_field_get(msg, msg->fields, 3) == NULL);

   fix_parser_free(parser);
   free(msg);
}
END_TEST

START_TEST(AddGetDelGroupTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create(&attrs, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   FIXField* field = NULL;
   FIXGroup* grp = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field);
   fail_unless(field != NULL);
   fail_unless(grp != NULL);
   fail_unless(field->size, 1);
   fail_unless(msg->used_groups == grp);
   fail_unless(parser->used_groups == 2);

   long val = 100;
   FIXField* field1 = fix_field_set(msg, NULL, new_fdescr(1, FIXFieldCategory_Value, FIXFieldValueType_String), (unsigned char*)&val, sizeof(val));
   fail_unless(field1 == NULL);
   fail_unless(parser->err_code == FIX_ERROR_FIELD_HAS_WRONG_TYPE);

   FIXField* field11 = NULL;
   FIXGroup* grp1 = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field11);
   fail_unless(field11 != NULL);
   fail_unless(grp1 != NULL);
   fail_unless(field11->size, 2);
   fail_unless(msg->used_groups == grp1);
   fail_unless(msg->used_groups->next == grp);
   fail_unless(parser->used_groups == 3);
   fail_unless(parser->group == NULL);

   FIXField* field2 = NULL;
   FIXGroup* grp2 = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field2);
   fail_unless(field2 != NULL);
   fail_unless(grp2 != NULL);
   fail_unless(field2->size, 3);
   fail_unless(msg->used_groups == grp2);
   fail_unless(msg->used_groups->next == grp1);
   fail_unless(msg->used_groups->next->next == grp);
   fail_unless(parser->used_groups == 4);
   fail_unless(parser->group == NULL);

   FIXField* field3 = NULL;
   FIXGroup* grp3 = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field3);
   fail_unless(grp3 != NULL);
   fail_unless(*(uint32_t*)field->data, 4);
   fail_unless(msg->used_groups == grp3);
   fail_unless(msg->used_groups->next == grp2);
   fail_unless(msg->used_groups->next->next == grp1);
   fail_unless(msg->used_groups->next->next->next == grp);
   fail_unless(parser->used_groups == 5);
   fail_unless(parser->group == NULL);

   FIXGroup* grp4 = fix_group_get(msg, NULL, 1, 4);
   fail_unless(grp4 == NULL);
   fail_unless(parser->err_code == FIX_ERROR_GROUP_WRONG_INDEX);

   fail_unless(fix_group_get(msg, NULL, 1, 3) == grp3);
   fail_unless(fix_group_get(msg, NULL, 1, 2) == grp2);
   fail_unless(fix_group_get(msg, NULL, 1, 1) == grp1);
   fail_unless(fix_group_get(msg, NULL, 1, 0) == grp);

   FIXGroups* grps = (FIXGroups*)field->data;
   fail_unless(field->size == 4);
   fail_unless(grps->group[0] == grp);
   fail_unless(grps->group[1] == grp1);
   fail_unless(grps->group[2] == grp2);
   fail_unless(grps->group[3] == grp3);

   fail_unless(parser->group == NULL);
   fail_unless(msg->used_groups == grp3);

   fail_unless(fix_group_del(msg, NULL, 1, 3) == FIX_SUCCESS);
   fail_unless(parser->group == grp3);
   fail_unless(parser->group->next == NULL);
   fail_unless(msg->used_groups == grp2);
   fail_unless(msg->used_groups->next == grp1);
   fail_unless(msg->used_groups->next->next == grp);
   fail_unless(msg->used_groups->next->next->next == NULL);

   fail_unless(field->size == 3);
   fail_unless(grps->group[0] == grp);
   fail_unless(grps->group[1] == grp1);
   fail_unless(grps->group[2] == grp2);
   fail_unless(grps->group[3] == NULL);

   fail_unless(fix_group_del(msg, NULL, 1, 1) == FIX_SUCCESS);
   fail_unless(parser->group == grp1);
   fail_unless(parser->group->next == grp3);
   fail_unless(parser->group->next->next == NULL);
   fail_unless(msg->used_groups == grp2);
   fail_unless(msg->used_groups->next == grp);
   fail_unless(msg->used_groups->next->next == NULL);

   fail_unless(field->size == 2);
   fail_unless(grps->group[0] == grp);
   fail_unless(grps->group[1] == grp2);
   fail_unless(grps->group[2] == NULL);
   fail_unless(grps->group[3] == NULL);

   fail_unless(fix_group_del(msg, NULL, 1, 0) == FIX_SUCCESS);
   fail_unless(parser->group == grp);
   fail_unless(parser->group->next == grp1);
   fail_unless(parser->group->next->next == grp3);
   fail_unless(msg->used_groups == grp2);
   fail_unless(msg->used_groups->next == NULL);

   fail_unless(field->size == 1);
   fail_unless(grps->group[0] == grp2);
   fail_unless(grps->group[1] == NULL);
   fail_unless(grps->group[2] == NULL);
   fail_unless(grps->group[3] == NULL);

   fail_unless(fix_group_del(msg, NULL, 1, 0) == FIX_SUCCESS);
   fail_unless(fix_field_get(msg, NULL, 1) == NULL);
   fail_unless(parser->group == grp2);
   fail_unless(parser->group->next == grp);
   fail_unless(parser->group->next->next == grp1);
   fail_unless(parser->group->next->next->next == grp3);
   fail_unless(msg->used_groups == NULL);

   fix_parser_free(parser);
   free(msg);
}
END_TEST

START_TEST(NestedGroupsTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create(&attrs, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   FIXField* field = NULL;
   FIXGroup* grp = fix_group_add(msg, NULL, new_fdescr(1, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &field);
   fail_unless(field != NULL);
   fail_unless(grp != NULL);
   fail_unless(field->size, 1);
   fail_unless(msg->used_groups == grp);
   fail_unless(parser->used_groups == 2);

   FIXField* nested_field = NULL;
   FIXGroup* nested_grp = fix_group_add(msg, grp, new_fdescr(65, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &nested_field);
   fail_unless(nested_field != NULL);
   fail_unless(nested_grp != NULL);
   fail_unless(nested_field->descr->category == FIXFieldCategory_Group);
   fail_unless(parser->used_groups == 3);

   fail_unless(msg->used_groups == nested_grp);
   fail_unless(msg->used_groups->next == grp);
   fail_unless(msg->used_groups->next->next == NULL);

   FIXField* nested_field1 = NULL;
   FIXGroup* nested_grp1 = fix_group_add(msg, nested_grp, new_fdescr(131, FIXFieldCategory_Group, FIXFieldValueType_NumInGroup), &nested_field1);
   fail_unless(nested_field1 != NULL);
   fail_unless(nested_grp1 != NULL);
   fail_unless(nested_field1->descr->category == FIXFieldCategory_Group);
   fail_unless(parser->used_groups == 4);

   fail_unless(msg->used_groups == nested_grp1);
   fail_unless(msg->used_groups->next == nested_grp);
   fail_unless(msg->used_groups->next->next == grp);
   fail_unless(msg->used_groups->next->next->next == NULL);

   fail_unless(fix_group_del(msg, nested_grp, 131, 0) == FIX_SUCCESS);

   fail_unless(msg->used_groups == nested_grp);
   fail_unless(msg->used_groups->next == grp);
   fail_unless(msg->used_groups->next->next == NULL);
   fail_unless(parser->group == nested_grp1);

   fail_unless(fix_group_del(msg, NULL, 1, 0) == FIX_SUCCESS);
   fail_unless(msg->used_groups == NULL);
   fail_unless(parser->group == grp);
   fail_unless(parser->group->next == nested_grp);
   fail_unless(parser->group->next->next == nested_grp1);
   fail_unless(parser->group->next->next->next == NULL);

   fix_parser_free(parser);
   free(msg);
}
END_TEST

Suite* make_fix_field_tests_suite()
{
   Suite* s = suite_create("fix_field");
   TCase* tc_core = tcase_create("Core");
   tcase_add_test(tc_core, SetTagTest);
   tcase_add_test(tc_core, DelTagTest);
   tcase_add_test(tc_core, GetTagTest);
   tcase_add_test(tc_core, AddGetDelGroupTest);
   tcase_add_test(tc_core, NestedGroupsTest);
   suite_add_tcase(s, tc_core);
   return s;
}