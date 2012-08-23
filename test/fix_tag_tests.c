/* @file   fix_tag_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/03/2012 02:41:02 PM
*/

#include <fix_parser.h>
#include <fix_parser_priv.h>
#include <fix_message_priv.h>
#include <fix_tag.h>

#include <check.h>
#include <stdlib.h>

FIXMessage* new_fake_message(FIXParser* parser)
{
   FIXMessage* msg = calloc(1, sizeof(FIXMessage));
   msg->tags = fix_parser_get_group(parser);
   msg->parser = parser;
   msg->pages = msg->curr_page = fix_parser_get_page(parser, 0);
   return msg;
}

START_TEST(SetTagTest)
{
   FIXParser* parser = new_fix_parser(512, 0, 2, 0, 2, 0, FIXParserFlags_Validate);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMessage* msg = new_fake_message(parser);

   long val = 1000;
   FIXTag* tag = fix_tag_set(msg, msg->tags, 1, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->fix_tags[1] == tag);
   fail_unless(tag->num == 1);
   fail_unless(tag->type == FIXTagType_Value);
   fail_unless(tag->next == NULL);
   fail_unless(*(long*)tag->data, val);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 + sizeof(FIXTag) + 4 + sizeof(val));
   fail_unless(msg->curr_page->next == NULL);

   FIXTag* tag11 = fix_tag_set(msg, msg->tags, 2, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->fix_tags[2] == tag11);
   fail_unless(tag11->num == 2);
   fail_unless(tag11->type == FIXTagType_Value);
   fail_unless(tag11->next == NULL);
   fail_unless(*(long*)tag11->data, val);

   FIXTag* tag12 = fix_tag_set(msg, msg->tags, 30, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->fix_tags[30] == tag12);
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

   free_fix_parser(parser);
   free(msg);
}
END_TEST

START_TEST(DelTagTest)
{
   FIXParser* parser = new_fix_parser(512, 0, 2, 0, 2, 0, FIXParserFlags_Validate);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMessage* msg = new_fake_message(parser);

   long val = 1000;
   FIXTag* tag = fix_tag_set(msg, msg->tags, 1, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->fix_tags[1] == tag);
   fail_unless(tag->num == 1);
   fail_unless(tag->type == FIXTagType_Value);
   fail_unless(tag->next == NULL);
   fail_unless(*(long*)tag->data, val);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 + sizeof(FIXTag) + 4 + sizeof(val));
   fail_unless(msg->curr_page->next == NULL);

   long val1 = 2000;
   FIXTag* tag1 = fix_tag_set(msg, msg->tags, 65, (unsigned char const*)&val1, sizeof(val1));
   fail_unless(msg->tags->fix_tags[1] == tag1);
   fail_unless(tag1->num == 65);
   fail_unless(tag1->type == FIXTagType_Value);
   fail_unless(tag1->next == tag);
   fail_unless(*(long*)tag1->data, val1);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 2 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   long val2 = 3000;
   FIXTag* tag2 = fix_tag_set(msg, msg->tags, 129, (unsigned char const*)&val2, sizeof(val2));
   fail_unless(msg->tags->fix_tags[1] == tag2);
   fail_unless(tag2->num == 129);
   fail_unless(tag2->type == FIXTagType_Value);
   fail_unless(tag2->next == tag1);
   fail_unless(*(long*)tag2->data, val2);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 3 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   long val3 = 4000;
   FIXTag* tag3 = fix_tag_set(msg, msg->tags, 193, (unsigned char const*)&val3, sizeof(val3));

   fail_unless(msg->tags->fix_tags[1] == tag3);
   fail_unless(tag3->num == 193);
   fail_unless(tag3->type == FIXTagType_Value);
   fail_unless(tag3->next == tag2);
   fail_unless(*(long*)tag2->data, val3);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   int res = fix_tag_del(msg, msg->tags, 1);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->tags->fix_tags[1] == tag3);
   fail_unless(msg->tags->fix_tags[1]->next == tag2);
   fail_unless(msg->tags->fix_tags[1]->next->next == tag1);
   fail_unless(msg->tags->fix_tags[1]->next->next->next == NULL);

   fail_unless(msg->curr_page->size == 512);
   fail_unless(msg->curr_page->offset == 4 * (4 + sizeof(FIXTag) + 4 + sizeof(val)));
   fail_unless(msg->curr_page->next == NULL);

   res = fix_tag_del(msg, msg->tags, 129);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->tags->fix_tags[1] == tag3);
   fail_unless(msg->tags->fix_tags[1]->next == tag1);
   fail_unless(msg->tags->fix_tags[1]->next->next == NULL);

   res = fix_tag_del(msg, msg->tags, 193);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->tags->fix_tags[1] == tag1);
   fail_unless(msg->tags->fix_tags[1]->next == NULL);

   res = fix_tag_del(msg, msg->tags, 65);
   fail_unless(res == FIX_SUCCESS);
   fail_unless(msg->tags->fix_tags[1] == NULL);

   free_fix_parser(parser);
   free(msg);
}
END_TEST

START_TEST(GetTagTest)
{
   FIXParser* parser = new_fix_parser(512, 0, 2, 0, 2, 0, FIXParserFlags_Validate);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXMessage* msg = new_fake_message(parser);

   long val = 1000;
   FIXTag* tag = fix_tag_set(msg, msg->tags, 1, (unsigned char const*)&val, sizeof(val));
   fail_unless(msg->tags->fix_tags[1] == tag);
   fail_unless(tag->num == 1);
   fail_unless(tag->type == FIXTagType_Value);
   fail_unless(tag->next == NULL);
   fail_unless(*(long*)tag->data, val);

   long val1 = 2000;
   FIXTag* tag1 = fix_tag_set(msg, msg->tags, 65, (unsigned char const*)&val1, sizeof(val1));
   fail_unless(msg->tags->fix_tags[1] == tag1);
   fail_unless(msg->tags->fix_tags[1]->next == tag);
   fail_unless(tag1->num == 65);
   fail_unless(tag1->type == FIXTagType_Value);
   fail_unless(tag1->next == tag);
   fail_unless(*(long*)tag1->data, val1);

   long val2 = 3000;
   FIXTag* tag2 = fix_tag_set(msg, msg->tags, 129, (unsigned char const*)&val2, sizeof(val2));
   fail_unless(msg->tags->fix_tags[1] == tag2);
   fail_unless(msg->tags->fix_tags[1]->next == tag1);
   fail_unless(msg->tags->fix_tags[1]->next->next == tag);
   fail_unless(tag2->num == 129);
   fail_unless(tag2->type == FIXTagType_Value);
   fail_unless(tag2->next == tag1);
   fail_unless(*(long*)tag2->data, val2);

   long val3 = 4000;
   FIXTag* tag3 = fix_tag_set(msg, msg->tags, 193, (unsigned char const*)&val3, sizeof(val3));
   fail_unless(msg->tags->fix_tags[1] == tag3);
   fail_unless(msg->tags->fix_tags[1]->next == tag2);
   fail_unless(msg->tags->fix_tags[1]->next->next == tag1);
   fail_unless(msg->tags->fix_tags[1]->next->next->next == tag);
   fail_unless(tag3->num == 193);
   fail_unless(tag3->type == FIXTagType_Value);
   fail_unless(tag3->next == tag2);
   fail_unless(*(long*)tag2->data, val3);

   long val4 = 4000;
   FIXTag* tag4 = fix_tag_set(msg, msg->tags, 2, (unsigned char const*)&val4, sizeof(val4));
   fail_unless(msg->tags->fix_tags[1] == tag3);
   fail_unless(msg->tags->fix_tags[1]->next == tag2);
   fail_unless(msg->tags->fix_tags[1]->next->next == tag1);
   fail_unless(msg->tags->fix_tags[1]->next->next->next == tag);
   fail_unless(msg->tags->fix_tags[2] == tag4);
   fail_unless(tag4->num == 2);
   fail_unless(tag4->type == FIXTagType_Value);
   fail_unless(tag4->next == NULL);
   fail_unless(*(long*)tag4->data, val4);

   fail_unless(fix_tag_get(msg, msg->tags, 1) == tag);
   fail_unless(fix_tag_get(msg, msg->tags, 129) == tag2);
   fail_unless(fix_tag_get(msg, msg->tags, 193) == tag3);
   fail_unless(fix_tag_get(msg, msg->tags, 2) == tag4);

   free_fix_parser(parser);
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
   suite_add_tcase(s, tc_core);
   return s;
}