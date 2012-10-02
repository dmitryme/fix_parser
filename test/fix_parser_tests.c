/* @file   fix_parser_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/03/2012 02:41:02 PM
*/

#include <check.h>

#include <fix_parser.h>
#include <fix_parser_priv.h>

START_TEST(CreateParserTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 2, 5, 2, 5, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);
   fail_unless(parser->flags == PARSER_FLAG_CHECK_ALL);
   fail_unless(parser->page != NULL);
   fail_unless(parser->page->next != NULL);
   fail_unless(parser->page->next->next == NULL);
   fail_unless(parser->used_pages == 0);
   fail_unless(parser->max_pages == 5);
   fail_unless(parser->page_size == 512);
   fail_unless(parser->group != NULL);
   fail_unless(parser->used_groups == 0);
   fail_unless(parser->max_groups == 5);

   fix_parser_free(parser);
}
END_TEST

START_TEST(SetErrorParserTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 2, 5, 2, 5, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);

   fix_parser_set_error(parser, FIX_ERROR_NO_MORE_PAGES, "No more pages available");

   fail_unless(parser->err_code == FIX_ERROR_NO_MORE_PAGES);
   fail_unless(!strcmp(parser->err_text, "No more pages available"));

   fix_parser_reset_error(parser);

   fail_unless(parser->err_code == 0);
   fail_unless(!strcmp(parser->err_text, ""));

   fix_parser_free(parser);
}
END_TEST

START_TEST(GetFreePageTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 2, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXPage* nextp = parser->page->next;
   FIXPage* p = fix_parser_alloc_page(parser, 0);

   fail_unless(p->next == NULL);
   fail_unless(parser->page == nextp);
   fail_unless(parser->used_pages == 1);

   FIXPage* p1 = fix_parser_alloc_page(parser, 0);
   fail_unless(p1 == nextp);
   fail_unless(p1->next == NULL);
   fail_unless(parser->used_pages == 2);

   FIXPage* p2 = fix_parser_alloc_page(parser, 0);
   fail_unless(parser->page == NULL);
   fail_unless(p2 != nextp);
   fail_unless(p1->next == NULL);
   fail_unless(parser->used_pages == 3);

   fix_parser_free_page(parser, p);

   fail_unless(parser->page == p);

   fix_parser_free_page(parser, p2);
   fail_unless(parser->page == p2);
   fail_unless(parser->page->next == p);

   fix_parser_free_page(parser, p1);
   fail_unless(parser->page == p1);
   fail_unless(parser->page->next == p2);

   fail_unless(parser->used_pages == 0);

   fix_parser_free(parser);
}
END_TEST

START_TEST(MaxPagesTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 2, 2, 2, 0, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXPage* nextp = parser->page->next;
   fail_unless(parser->page != NULL);

   FIXPage* p = fix_parser_alloc_page(parser, 0);
   fail_unless(p->next == NULL);
   fail_unless(parser->page == nextp);
   fail_unless(parser->used_pages == 1);

   FIXPage* p1 = fix_parser_alloc_page(parser, 0);
   fail_unless(p1 == nextp);
   fail_unless(p1->next == NULL);
   fail_unless(parser->used_pages == 2);

   FIXPage* p2 = fix_parser_alloc_page(parser, 0);
   fail_unless(p2 == NULL);
   fail_unless(parser->err_code == FIX_ERROR_NO_MORE_PAGES);
   fail_unless(parser->page == NULL);
   fail_unless(parser->used_pages == 2);

   fix_parser_free_page(parser, p);

   fail_unless(parser->page == p);
   fail_unless(parser->used_pages == 1);

   p = fix_parser_alloc_page(parser, 0);

   fail_unless(p->next == NULL);
   fail_unless(parser->page == NULL);
   fail_unless(parser->used_pages == 2);

   fix_parser_free(parser);
}
END_TEST

START_TEST(MaxGroupsTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 2, 0, 2, 2, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   FIXGroup* nextg = parser->group->next;
   fail_unless(parser->group != NULL);

   FIXGroup* g = fix_parser_alloc_group(parser);
   fail_unless(g->next == NULL);
   fail_unless(parser->group == nextg);
   fail_unless(parser->used_groups == 1);

   FIXGroup* g1 = fix_parser_alloc_group(parser);
   fail_unless(g1 == nextg);
   fail_unless(g1->next == NULL);
   fail_unless(parser->used_groups == 2);

   FIXGroup* p2 = fix_parser_alloc_group(parser);
   fail_unless(p2 == NULL);
   fail_unless(parser->err_code == FIX_ERROR_NO_MORE_GROUPS);
   fail_unless(parser->group == NULL);
   fail_unless(parser->used_groups == 2);

   fix_parser_free_group(parser, g);

   g = fix_parser_alloc_group(parser);
   fail_unless(g->next == NULL);
   fail_unless(parser->group == NULL);
   fail_unless(parser->used_groups == 2);

   fix_parser_free(parser);
}
END_TEST

START_TEST(MaxPageSizeTest)
{
   {
      FIXParser* parser = fix_parser_create(512, 0, 1, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
      fail_unless(parser != NULL);
      fail_unless(parser->err_code == 0);

      FIXPage* p = fix_parser_alloc_page(parser, 0);
      fail_unless(p->next == NULL);
      fail_unless(parser->used_pages == 1);
      fail_unless(p->size == 512);

      FIXPage* p1 = fix_parser_alloc_page(parser, 1024);
      fail_unless(p1->next == NULL);
      fail_unless(parser->used_pages == 2);
      fail_unless(p1->size == 1024);

      FIXPage* p2 = fix_parser_alloc_page(parser, 10240);
      fail_unless(p2->next == NULL);
      fail_unless(parser->used_pages == 3);
      fail_unless(p2->size == 10240);

      fix_parser_free(parser);
   }
   {
      FIXParser* parser = fix_parser_create(512, 512, 1, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
      fail_unless(parser != NULL);
      fail_unless(parser->err_code == 0);

      FIXPage* p = fix_parser_alloc_page(parser, 0);
      fail_unless(p != NULL);
      fail_unless(parser->used_pages == 1);
      fail_unless(p->size == 512);

      FIXPage* p1 = fix_parser_alloc_page(parser, 500);
      fail_unless(p1 != NULL);
      fail_unless(parser->used_pages == 2);
      fail_unless(p->size == 512);

      FIXPage* p2 = fix_parser_alloc_page(parser, 513);
      fail_unless(p2 == NULL);
      fail_unless(parser->err_code == FIX_ERROR_TOO_BIG_PAGE);
      fail_unless(parser->used_pages == 2);

      fix_parser_free(parser);
   }
}
END_TEST

START_TEST(ParseFieldTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 1, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   char buff[] = "8=FIX4.4|35=D|41=QWERTY|21=123";
   char const* begin = NULL;
   char const* end = NULL;
   int64_t num = parse_field(parser, buff, strlen(buff), '|', &begin, &end);
   fail_unless(num == 8);
   fail_unless(*begin == 'F');
   fail_unless(*end == '|');

   char buff1[] = "A=FIX4.4|35=D|41=QWERTY|21=123";
   num = parse_field(parser, buff1, strlen(buff1), '|', &begin, &end);
   fail_unless(num == FIX_FAILED);
   fail_unless(parser->err_code == FIX_ERROR_INVALID_ARGUMENT);
}
END_TEST

START_TEST(ParseBeginStringTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 1, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   fail_unless(fix_protocol_init(parser, "fix_descr/fix.4.4.xml") == FIX_SUCCESS);

   {
      char buff[] = "A12=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      FIXMsg* msg = parse_fix(parser, buff, strlen(buff), '|');
      fail_unless(msg == NULL);
      fail_unless(parser->err_code == FIX_ERROR_PARSE_MSG);
   }
   {
      char buff[] = "1=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      FIXMsg* msg = parse_fix(parser, buff, strlen(buff), '|');
      fail_unless(msg == NULL);
      fail_unless(parser->err_code == FIX_ERROR_WRONG_FIELD);
   }
   {
      char buff[] = "8=FIX.4.5|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      FIXMsg* msg = parse_fix(parser, buff, strlen(buff), '|');
      fail_unless(msg == NULL);
      fail_unless(parser->err_code == FIX_ERROR_WRONG_PROTOCOL_VER);
   }
   {
      char buff[] = "8=FIXT.1.1|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      FIXMsg* msg = parse_fix(parser, buff, strlen(buff), '|');
      fail_unless(msg == NULL);
      fail_unless(parser->err_code == FIX_ERROR_WRONG_PROTOCOL_VER);
   }
}
END_TEST

START_TEST(ParseBodyLengthTest)
{
   FIXParser* parser = fix_parser_create(512, 0, 1, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->err_code == 0);

   fail_unless(fix_protocol_init(parser, "fix_descr/fix.4.4.xml") == FIX_SUCCESS);

   {
      char buff[] = "8=FIX.4.4|10=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      FIXMsg* msg = parse_fix(parser, buff, strlen(buff), '|');
      fail_unless(msg == NULL);
      fail_unless(parser->err_code == FIX_ERROR_WRONG_FIELD);
   }
   {
      char buff[] = "8=FIX.4.4|9=228A|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      FIXMsg* msg = parse_fix(parser, buff, strlen(buff), '|');
      fail_unless(msg == NULL);
      fail_unless(parser->err_code == FIX_ERROR_PARSE_MSG);
   }
}
END_TEST

Suite* make_fix_parser_tests_suite()
{
   Suite* s = suite_create("fix_parser");
   TCase* tc_core = tcase_create("Core");
   tcase_add_test(tc_core, CreateParserTest);
   tcase_add_test(tc_core, SetErrorParserTest);
   tcase_add_test(tc_core, GetFreePageTest);
   tcase_add_test(tc_core, MaxPagesTest);
   tcase_add_test(tc_core, MaxGroupsTest);
   tcase_add_test(tc_core, MaxPageSizeTest);
   tcase_add_test(tc_core, ParseFieldTest);
   tcase_add_test(tc_core, ParseBeginStringTest);
   tcase_add_test(tc_core, ParseBodyLengthTest);
   suite_add_tcase(s, tc_core);
   return s;
}