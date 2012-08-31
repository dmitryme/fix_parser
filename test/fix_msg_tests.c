/* @file   fix_msg_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/27/2012 05:09:24 PM
*/

#include <fix_msg.h>
#include <fix_parser.h>
#include <fix_msg_priv.h>
#include <fix_parser_priv.h>

#include <check.h>

START_TEST(CreateMsgTest)
{
   FIXParser* p = fix_parser_create(512, 0, 2, 0, 2, 0, 0);
   fail_unless(p != NULL);
   FIXGroup* group = p->group;
   fail_unless(fix_protocol_init(p, "fix44.xml") == FIX_SUCCESS);

   FIXMsg* msg = fix_msg_create(p, FIX44, "D");
   fail_unless(msg != NULL);
   fail_unless(msg->used_groups == group);

   char buff[10];
   fail_unless(fix_msg_get_string(msg, NULL, 8, buff, sizeof(buff)) == 7);
   fail_unless(!strcmp(buff, "FIX.4.4"));
   char msgType[10];
   fail_unless(fix_msg_get_string(msg, NULL, 35, msgType, sizeof(msgType)) == 1);
   fail_unless(!strcmp(msgType, "D"));

   fail_unless(fix_msg_set_string(msg, NULL, 11, "ClOrdID") == FIX_SUCCESS);

   fix_msg_free(msg);
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