/* @file   fix_parser_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 09/13/2012 03:23:02 PM
*/

#include <check.h>
#include <fix_utils.h>
#include <fix_types.h>

START_TEST(i64toa_Test)
{
   char buff[10] = {};
   fail_unless(fix_utils_i64toa(100, buff, sizeof(buff), 0) == 3);
   fail_unless(!strcmp(buff, "100"));

   char buff1[4];
   fail_unless(fix_utils_i64toa(77777, buff1, sizeof(buff1), 0) == 4);
   fail_unless(!strncmp(buff1, "7777", 4));

   char buff2[3];
   fail_unless(fix_utils_i64toa(99, buff2, sizeof(buff2), '0') == 3);
   fail_unless(!strncmp(buff2, "099", 3));

   char buff3[7];
   fail_unless(fix_utils_i64toa(-77777, buff3, sizeof(buff3), 0) == 6);
   fail_unless(!strncmp(buff3, "-77777", 7));

   char buff4[4];
   fail_unless(fix_utils_i64toa(-37, buff4, sizeof(buff4), '0') == 4);
   fail_unless(!strncmp(buff4, "-037", 4));

   char buff5[7];
   fail_unless(fix_utils_i64toa(-37, buff5, sizeof(buff5), '0') == 7);
   fail_unless(!strncmp(buff5, "-000037", 7));
}
END_TEST

START_TEST(dtoa_Test)
{
   char buff[10] = {};
   fail_unless(fix_utils_dtoa(100.12, buff, sizeof(buff)) == 6);
   fail_unless(!strcmp(buff, "100.12"));

   char buff1[10] = {};
   fail_unless(fix_utils_dtoa(-3456.00, buff1, sizeof(buff1)) == 5);
   fail_unless(!strcmp(buff1, "-3456"));

   char buff2[3];
   fail_unless(fix_utils_dtoa(-3456.12, buff2, sizeof(buff2)) == 3);
   fail_unless(!strncmp(buff2, "-34", 3));
}
END_TEST

START_TEST(atoi64_Test)
{
   char str[] = "1000";
   int64_t val = 0;
   fail_unless(fix_utils_atoi64(str, strlen(str), 0, &val) == 4);
   fail_unless(val == 1000);

   char str1[] = "-123456";
   int64_t val1 = 0;
   fail_unless(fix_utils_atoi64(str1, strlen(str1), 0, &val1) == 7);
   fail_unless(val1 == -123456);

   char str2[] = "00123";
   int64_t val2 = 0;
   fail_unless(fix_utils_atoi64(str2, strlen(str2), 0, &val2) == 5);
   fail_unless(val2 == 123);

   char str3[] = "-00123";
   int64_t val3 = 0;
   fail_unless(fix_utils_atoi64(str3, strlen(str3), 0, &val3) == 6);
   fail_unless(val3 == -123);

   char str4[] = "1000=";
   int64_t val4 = 0;
   fail_unless(fix_utils_atoi64(str4, strlen(str4), '=', &val4) == 4);
   fail_unless(val4 == 1000);

   char str5[] = "1000=";
   int64_t val5 = 0;
   fail_unless(fix_utils_atoi64(str5, 3, '=', &val5) == 3);
   fail_unless(val5 == 100);

   char str6[] = "1000\00134=";
   int64_t val6 = 0;
   fail_unless(fix_utils_atoi64(str6, strlen(str6), '=', &val6) == FIX_FAILED);
   fail_unless(val6 == 1000);
}
END_TEST

START_TEST(atod_Test)
{
   char str[] = "1000.12";
   double val = 0;
   fail_unless(fix_utils_atod(str, strlen(str), 0, &val) == 7);
   fail_unless(val == 1000.12);

   char str1[] = "-123456.345";
   double val1 = 0;
   fail_unless(fix_utils_atod(str1, strlen(str1), 0, &val1) == 11);
   fail_unless(val1 == -123456.345);

   char str2[] = "00123.987";
   double val2 = 0;
   fail_unless(fix_utils_atod(str2, strlen(str2), 0, &val2) == 9);
   fail_unless(val2 == 123.987);

   char str3[] = "-00123.456";
   double val3 = 0;
   fail_unless(fix_utils_atod(str3, strlen(str3), 0, &val3) == 10);
   fail_unless(val3 == -123.456);

   char str4[] = "123.456\00134=";
   double val4 = 0;
   fail_unless(fix_utils_atod(str4, strlen(str4), 1, &val4) == 7);
   fail_unless(val4 == 123.456);
}
END_TEST

Suite* make_fix_utils_tests_suite()
{
   Suite* s = suite_create("fix_utils");
   TCase* tc_core = tcase_create("Core");
   tcase_add_test(tc_core, i64toa_Test);
   tcase_add_test(tc_core, dtoa_Test);
   tcase_add_test(tc_core, atoi64_Test);
   tcase_add_test(tc_core, atod_Test);
   suite_add_tcase(s, tc_core);
   return s;
}
