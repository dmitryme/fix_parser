/* @file   fix_tag_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/03/2012 02:41:02 PM
*/

#include <fix_parser.h>
#include <fix_parser_priv.h>
#include <fix_msg_priv.h>
#include <fix_tag.h>

#include <check.h>
#include <stdlib.h>

FIXMsg* new_fake_message(FIXParser* parser)
{
   FIXMsg* msg = calloc(1, sizeof(FIXMsg));
   msg->tags = fix_parser_alloc_group(parser);
   msg->parser = parser;
   msg->pages = msg->curr_page = fix_parser_alloc_page(parser, 0);
   return msg;
}

START_TEST(SetTagTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 2, 0, 2, 0, FIXParserFlags_Validate);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   long val = 1000;
   FIXTag* tag = fix_tag_set(msg, msg->tags, 1, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->tags[1] == tag);
   fail_unless(tag->num == 1);
   fail_unless(tag->type == FIXTagType_Value);
   fail_unless(tag->next == NULL);
   fail_unless(*(long*)tag->data, val);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 + sizeof(FIXTag) + 4 + sizeof(val));
   fail_unless(msg->curr_page->next == NULL);

   FIXTag* tag11 = fix_tag_set(msg, msg->tags, 2, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->tags[2] == tag11);
   fail_unless(tag11->num == 2);
   fail_unless(tag11->type == FIXTagType_Value);
   fail_unless(tag11->next == NULL);
   fail_unless(*(long*)tag11->data, val);

   FIXTag* tag12 = fix_tag_set(msg, msg->tags, 30, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->tags[30] == tag12);
   fail_unless(tag12->num == 30);
   fail_unless(tag12->type == FIXTagType_Value);
   fail_unless(tag12->next == NULL);
   fail_unless(*(long*)tag12->data, val);

   val = 2000;
   FIXTag* tag1 = fix_tag_set(msg, msg->tags, 1, (unsigned char const*)&val, sizeof(val));
   fail_unless(tag == tag1);
   fail_unless(tag1->num == 1);
   fail_unless(tag1->type == FIXTagType_Value);
   fail_unless(tag1->next == NULL);
   fail_unless(*(long*)tag1->data, val);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 3 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   int val1 = 64;
   FIXTag* tag2 = fix_tag_set(msg, msg->tags, 1, (unsigned char const*)&val1, sizeof(val1));
   fail_unless(tag2 == tag);
   fail_unless(tag2->num == 1);
   fail_unless(tag2->type == FIXTagType_Value);
   fail_unless(tag2->next == NULL);
   fail_unless(*(int*)tag2->data, val1);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 3 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   char const* txt = "Hello world!";
   FIXTag* tag3 = fix_tag_set(msg, msg->tags, 1, (unsigned char const*)txt, strlen(txt));
   fail_unless(tag3 == tag);
   fail_unless(tag3->num == 1);
   fail_unless(tag3->type == FIXTagType_Value);
   fail_unless(tag3->next == NULL);
   fail_unless(!strcmp((char const*)tag3->data, txt));

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 3 * (4 + sizeof(FIXTag) + 4 + sizeof(val)) + 4 + strlen(txt));
   fail_unless(msg->curr_page->next == NULL);

   fix_parser_free(parser);
   free(msg);
}
END_TEST

START_TEST(DelTagTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 2, 0, 2, 0, FIXParserFlags_Validate);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   long val = 1000;
   FIXTag* tag = fix_tag_set(msg, msg->tags, 1, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->tags[1] == tag);
   fail_unless(tag->num == 1);
   fail_unless(tag->type == FIXTagType_Value);
   fail_unless(tag->next == NULL);
   fail_unless(*(long*)tag->data, val);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 + sizeof(FIXTag) + 4 + sizeof(val));
   fail_unless(msg->curr_page->next == NULL);

   long val1 = 2000;
   FIXTag* tag1 = fix_tag_set(msg, msg->tags, 65, (unsigned char const*)&val1, sizeof(val1));
   fail_unless(msg->tags->tags[1] == tag1);
   fail_unless(tag1->num == 65);
   fail_unless(tag1->type == FIXTagType_Value);
   fail_unless(tag1->next == tag);
   fail_unless(*(long*)tag1->data, val1);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 2 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   long val2 = 3000;
   FIXTag* tag2 = fix_tag_set(msg, msg->tags, 129, (unsigned char const*)&val2, sizeof(val2));
   fail_unless(msg->tags->tags[1] == tag2);
   fail_unless(tag2->num == 129);
   fail_unless(tag2->type == FIXTagType_Value);
   fail_unless(tag2->next == tag1);
   fail_unless(*(long*)tag2->data, val2);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 3 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   long val3 = 4000;
   FIXTag* tag3 = fix_tag_set(msg, msg->tags, 193, (unsigned char const*)&val3, sizeof(val3));

   fail_unless(msg->tags->tags[1] == tag3);
   fail_unless(tag3->num == 193);
   fail_unless(tag3->type == FIXTagType_Value);
   fail_unless(tag3->next == tag2);
   fail_unless(*(long*)tag2->data, val3);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   int res = fix_tag_del(msg, msg->tags, 1);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->tags->tags[1] == tag3);
   fail_unless(msg->tags->tags[1]->next == tag2);
   fail_unless(msg->tags->tags[1]->next->next == tag1);
   fail_unless(msg->tags->tags[1]->next->next->next == NULL);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   res = fix_tag_del(msg, msg->tags, 129);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->tags->tags[1] == tag3);
   fail_unless(msg->tags->tags[1]->next == tag1);
   fail_unless(msg->tags->tags[1]->next->next == NULL);

   res = fix_tag_del(msg, msg->tags, 193);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->tags->tags[1] == tag1);
   fail_unless(msg->tags->tags[1]->next == NULL);

   res = fix_tag_del(msg, msg->tags, 65);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->tags->tags[1] == NULL);

   fix_parser_free(parser);
   free(msg);
}
END_TEST

START_TEST(GetTagTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 2, 0, 2, 0, FIXParserFlags_Validate);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   long val = 1000;
   FIXTag* tag = fix_tag_set(msg, msg->tags, 1, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->tags[1] == tag);
   fail_unless(tag->num == 1);
   fail_unless(tag->type == FIXTagType_Value);
   fail_unless(tag->next == NULL);
   fail_unless(*(long*)tag->data, val);

   long val1 = 2000;
   FIXTag* tag1 = fix_tag_set(msg, msg->tags, 65, (unsigned char const*)&val1, sizeof(val1));
   fail_unless(msg->tags->tags[1] == tag1);
   fail_unless(msg->tags->tags[1]->next == tag);
   fail_unless(tag1->num == 65);
   fail_unless(tag1->type == FIXTagType_Value);
   fail_unless(tag1->next == tag);
   fail_unless(*(long*)tag1->data, val1);

   long val2 = 3000;
   FIXTag* tag2 = fix_tag_set(msg, msg->tags, 129, (unsigned char const*)&val2, sizeof(val2));
   fail_unless(msg->tags->tags[1] == tag2);
   fail_unless(msg->tags->tags[1]->next == tag1);
   fail_unless(msg->tags->tags[1]->next->next == tag);
   fail_unless(tag2->num == 129);
   fail_unless(tag2->type == FIXTagType_Value);
   fail_unless(tag2->next == tag1);
   fail_unless(*(long*)tag2->data, val2);

   long val3 = 4000;
   FIXTag* tag3 = fix_tag_set(msg, msg->tags, 193, (unsigned char const*)&val3, sizeof(val3));
   fail_unless(msg->tags->tags[1] == tag3);
   fail_unless(msg->tags->tags[1]->next == tag2);
   fail_unless(msg->tags->tags[1]->next->next == tag1);
   fail_unless(msg->tags->tags[1]->next->next->next == tag);
   fail_unless(tag3->num == 193);
   fail_unless(tag3->type == FIXTagType_Value);
   fail_unless(tag3->next == tag2);
   fail_unless(*(long*)tag2->data, val3);

   long val4 = 4000;
   FIXTag* tag4 = fix_tag_set(msg, msg->tags, 2, (unsigned char const*)&val4, sizeof(val4));
   fail_unless(msg->tags->tags[1] == tag3);
   fail_unless(msg->tags->tags[1]->next == tag2);
   fail_unless(msg->tags->tags[1]->next->next == tag1);
   fail_unless(msg->tags->tags[1]->next->next->next == tag);
   fail_unless(msg->tags->tags[2] == tag4);
   fail_unless(tag4->num == 2);
   fail_unless(tag4->type == FIXTagType_Value);
   fail_unless(tag4->next == NULL);
   fail_unless(*(long*)tag4->data, val4);

   fail_unless(fix_tag_get(msg, msg->tags, 1) == tag);
   fail_unless(fix_tag_get(msg, msg->tags, 129) == tag2);
   fail_unless(fix_tag_get(msg, msg->tags, 193) == tag3);
   fail_unless(fix_tag_get(msg, msg->tags, 2) == tag4);

   fail_unless(fix_tag_get(msg, msg->tags, 3) == NULL);

   fix_parser_free(parser);
   free(msg);
}
END_TEST

START_TEST(AddGetDelGroupTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 2, 0, 2, 0, FIXParserFlags_Validate);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   FIXGroup* grp = fix_group_add(msg, msg->tags, 1);
   fail_unless(grp != NULL);
   FIXTag* tag = fix_tag_get(msg, msg->tags, 1);
   fail_unless(tag != NULL);
   fail_unless(*(uint32_t*)tag->data, 1);
   fail_unless(msg->used_groups == grp);
   fail_unless(parser->used_groups == 2);

   long val = 100;
   FIXTag* tag1 = fix_tag_set(msg, msg->tags, 1, (unsigned char*)&val, sizeof(val));
   fail_unless(tag1 == NULL);
   fail_unless(parser->err_code == FIX_ERROR_TAG_HAS_WRONG_TYPE);

   FIXGroup* grp1 = fix_group_add(msg, msg->tags, 1);
   fail_unless(grp1 != NULL);
   fail_unless(*(uint32_t*)tag->data, 2);
   fail_unless(msg->used_groups == grp1);
   fail_unless(msg->used_groups->next == grp);
   fail_unless(parser->used_groups == 3);
   fail_unless(parser->group == NULL);

   FIXGroup* grp2 = fix_group_add(msg, msg->tags, 1);
   fail_unless(grp2 != NULL);
   fail_unless(*(uint32_t*)tag->data, 3);
   fail_unless(msg->used_groups == grp2);
   fail_unless(msg->used_groups->next == grp1);
   fail_unless(msg->used_groups->next->next == grp);
   fail_unless(parser->used_groups == 4);
   fail_unless(parser->group == NULL);

   FIXGroup* grp3 = fix_group_add(msg, msg->tags, 1);
   fail_unless(grp3 != NULL);
   fail_unless(*(uint32_t*)tag->data, 4);
   fail_unless(msg->used_groups == grp3);
   fail_unless(msg->used_groups->next == grp2);
   fail_unless(msg->used_groups->next->next == grp1);
   fail_unless(msg->used_groups->next->next->next == grp);
   fail_unless(parser->used_groups == 5);
   fail_unless(parser->group == NULL);

   FIXGroup* grp4 = fix_group_get(msg, msg->tags, 1, 4);
   fail_unless(grp4 == NULL);
   fail_unless(parser->err_code == FIX_ERROR_GROUP_WRONG_INDEX);

   fail_unless(fix_group_get(msg, msg->tags, 1, 3) == grp3);
   fail_unless(fix_group_get(msg, msg->tags, 1, 2) == grp2);
   fail_unless(fix_group_get(msg, msg->tags, 1, 1) == grp1);
   fail_unless(fix_group_get(msg, msg->tags, 1, 0) == grp);

   FIXGroups* grps = (FIXGroups*)tag->data;
   fail_unless(grps->cnt == 4);
   fail_unless(grps->group[0] == grp);
   fail_unless(grps->group[1] == grp1);
   fail_unless(grps->group[2] == grp2);
   fail_unless(grps->group[3] == grp3);

   fail_unless(parser->group == NULL);
   fail_unless(msg->used_groups == grp3);

   fail_unless(fix_group_del(msg, msg->tags, 1, 3) == FIX_SUCCESS);
   fail_unless(parser->group == grp3);
   fail_unless(parser->group->next == NULL);
   fail_unless(msg->used_groups == grp2);
   fail_unless(msg->used_groups->next == grp1);
   fail_unless(msg->used_groups->next->next == grp);
   fail_unless(msg->used_groups->next->next->next == NULL);

   fail_unless(grps->cnt == 3);
   fail_unless(grps->group[0] == grp);
   fail_unless(grps->group[1] == grp1);
   fail_unless(grps->group[2] == grp2);
   fail_unless(grps->group[3] == NULL);

   fail_unless(fix_group_del(msg, msg->tags, 1, 1) == FIX_SUCCESS);
   fail_unless(parser->group == grp1);
   fail_unless(parser->group->next == grp3);
   fail_unless(parser->group->next->next == NULL);
   fail_unless(msg->used_groups == grp2);
   fail_unless(msg->used_groups->next == grp);
   fail_unless(msg->used_groups->next->next == NULL);

   fail_unless(grps->cnt == 2);
   fail_unless(grps->group[0] == grp);
   fail_unless(grps->group[1] == grp2);
   fail_unless(grps->group[2] == NULL);
   fail_unless(grps->group[3] == NULL);

   fail_unless(fix_group_del(msg, msg->tags, 1, 0) == FIX_SUCCESS);
   fail_unless(parser->group == grp);
   fail_unless(parser->group->next == grp1);
   fail_unless(parser->group->next->next == grp3);
   fail_unless(msg->used_groups == grp2);
   fail_unless(msg->used_groups->next == NULL);

   fail_unless(grps->cnt == 1);
   fail_unless(grps->group[0] == grp2);
   fail_unless(grps->group[1] == NULL);
   fail_unless(grps->group[2] == NULL);
   fail_unless(grps->group[3] == NULL);

   fail_unless(fix_group_del(msg, msg->tags, 1, 0) == FIX_SUCCESS);
   fail_unless(fix_tag_get(msg, msg->tags, 1) == NULL);
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
   FIXParser* parser = fix_parser_create(512, 0, 2, 0, 2, 0, FIXParserFlags_Validate);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMsg* msg = new_fake_message(parser);

   FIXGroup* grp = fix_group_add(msg, msg->tags, 1);
   fail_unless(grp != NULL);
   FIXTag* tag = fix_tag_get(msg, msg->tags, 1);
   fail_unless(tag != NULL);
   fail_unless(*(uint32_t*)tag->data, 1);
   fail_unless(msg->used_groups == grp);
   fail_unless(parser->used_groups == 2);

   FIXGroup* nested_grp = fix_group_add(msg, grp, 65);
   fail_unless(nested_grp != NULL);
   FIXTag* nested_tag = fix_tag_get(msg, grp, 65);
   fail_unless(nested_tag != NULL);
   fail_unless(nested_tag->type == FIXTagType_Group);
   fail_unless(parser->used_groups == 3);

   fail_unless(msg->used_groups == nested_grp);
   fail_unless(msg->used_groups->next == grp);
   fail_unless(msg->used_groups->next->next == NULL);

   FIXGroup* nested_grp1 = fix_group_add(msg, nested_grp, 131);
   fail_unless(nested_grp1 != NULL);
   FIXTag* nested_tag1 = fix_tag_get(msg, nested_grp, 131);
   fail_unless(nested_tag1 != NULL);
   fail_unless(nested_tag1->type == FIXTagType_Group);
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

   fail_unless(fix_group_del(msg, msg->tags, 1, 0) == FIX_SUCCESS);
   fail_unless(msg->used_groups == NULL);
   fail_unless(parser->group == grp);
   fail_unless(parser->group->next == nested_grp);
   fail_unless(parser->group->next->next == nested_grp1);
   fail_unless(parser->group->next->next->next == NULL);

   fix_parser_free(parser);
   free(msg);
}
END_TEST

Suite* make_fix_tag_tests_suite()
{
   Suite* s = suite_create("fix_tag");
   TCase* tc_core = tcase_create("Core");
   tcase_add_test(tc_core, SetTagTest);
   tcase_add_test(tc_core, DelTagTest);
   tcase_add_test(tc_core, GetTagTest);
   tcase_add_test(tc_core, AddGetDelGroupTest);
   tcase_add_test(tc_core, NestedGroupsTest);
   suite_add_tcase(s, tc_core);
   return s;
}