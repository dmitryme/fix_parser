/* @file   fix_msg_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/27/2012 05:09:24 PM
*/

#include <fix_msg.h>
#include <fix_parser.h>

#include <check.h>

START_TEST(CreateMsgTest)
{
   FIXParser* p = fix_parser_create(512, 0, 2, 0, 2, 0, 0);
   fail_unless(p != NULL);
   fix_parser_free(p);
}
END_TEST

Suite* make_fix_msg_tests_suite()
{
   Suite* s = suite_create("fix_msg");
   TCase* tc_core = tcase_create("Core");
   tcase_add_test(tc_core, CreateMsgTest);
   suite_add_tcase(s, tc_core);
   return s;
}