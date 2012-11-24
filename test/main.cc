/* @file   main.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/03/2012 02:41:31 PM
*/

#include <check.h>
#include <stdlib.h>

extern Suite* make_fix_field_tests_suite();
extern Suite* make_fix_parser_tests_suite();
extern Suite* make_fix_msg_tests_suite();
extern Suite* make_fix_utils_tests_suite();
extern Suite* make_fix_protocol_tests_suite();
extern Suite* make_fix_parser_tests2_suite();

int main()
{
   SRunner* sr = srunner_create(make_fix_protocol_tests_suite());
   srunner_add_suite(sr, make_fix_parser_tests_suite());
   srunner_add_suite(sr, make_fix_field_tests_suite());
   srunner_add_suite(sr, make_fix_msg_tests_suite());
   srunner_add_suite(sr, make_fix_utils_tests_suite());
   srunner_add_suite(sr, make_fix_parser_tests2_suite());
   srunner_run_all(sr, CK_NORMAL);
   int number_failed = srunner_ntests_failed(sr);
   return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
