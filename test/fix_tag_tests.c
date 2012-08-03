/// @file   fix_tag_tests.c
/// @author Dmitry S. Melnikov, dmitryme@gmail.com
/// @date   Created on: 08/03/2012 02:41:02 PM

#include <check.h>

START_TEST(test1)
{
}
END_TEST

Suite* make_fix_tag_tests_suite()
{
   Suite* s = suite_create("fix_parser");
   TCase* tc_core = tcase_create("Core");
   tcase_add_test(tc_core, test1);
   suite_add_tcase(s, tc_core);
   return s;
}