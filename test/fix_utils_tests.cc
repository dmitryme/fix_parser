/* @file   fix_parser_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 09/13/2012 03:23:02 PM
*/

extern "C"
{
#include <fix_utils.h>
#include <fix_types.h>
}
#include <gtest/gtest.h>

TEST(FixUtilsTests, i64toa_Test)
{
   char buff[10] = {};
   ASSERT_TRUE(fix_utils_i64toa(100, buff, sizeof(buff), 0) == 3);
   ASSERT_TRUE(!strcmp(buff, "100"));

   char buff1[4];
   ASSERT_TRUE(fix_utils_i64toa(77777, buff1, sizeof(buff1), 0) == 4);
   ASSERT_TRUE(!strncmp(buff1, "7777", 4));

   char buff2[3];
   ASSERT_TRUE(fix_utils_i64toa(99, buff2, sizeof(buff2), '0') == 3);
   ASSERT_TRUE(!strncmp(buff2, "099", 3));

   char buff3[7];
   ASSERT_TRUE(fix_utils_i64toa(-77777, buff3, sizeof(buff3), 0) == 6);
   ASSERT_TRUE(!strncmp(buff3, "-77777", 7));

   char buff4[4];
   ASSERT_TRUE(fix_utils_i64toa(-37, buff4, sizeof(buff4), '0') == 4);
   ASSERT_TRUE(!strncmp(buff4, "-037", 4));

   char buff5[7];
   ASSERT_TRUE(fix_utils_i64toa(-37, buff5, sizeof(buff5), '0') == 7);
   ASSERT_TRUE(!strncmp(buff5, "-000037", 7));
}

TEST(FixUtilsTests, dtoa_Test)
{
   char buff[10] = {};
   ASSERT_TRUE(fix_utils_dtoa(100.12, buff, sizeof(buff)) == 6);
   ASSERT_TRUE(!strcmp(buff, "100.12"));

   char buff1[10] = {};
   ASSERT_TRUE(fix_utils_dtoa(-3456.00, buff1, sizeof(buff1)) == 5);
   ASSERT_TRUE(!strcmp(buff1, "-3456"));

   char buff2[3];
   ASSERT_TRUE(fix_utils_dtoa(-3456.12, buff2, sizeof(buff2)) == 3);
   ASSERT_TRUE(!strncmp(buff2, "-34", 3));
}


TEST(FixUtilsTests, atoi64_Test)
{
   char str[] = "1000";
   int64_t val = 0;
   ASSERT_TRUE(fix_utils_atoi64(str, strlen(str), 0, &val) == 4);
   ASSERT_TRUE(val == 1000);

   char str1[] = "-123456";
   int64_t val1 = 0;
   ASSERT_TRUE(fix_utils_atoi64(str1, strlen(str1), 0, &val1) == 7);
   ASSERT_TRUE(val1 == -123456);

   char str2[] = "00123";
   int64_t val2 = 0;
   ASSERT_TRUE(fix_utils_atoi64(str2, strlen(str2), 0, &val2) == 5);
   ASSERT_TRUE(val2 == 123);

   char str3[] = "-00123";
   int64_t val3 = 0;
   ASSERT_TRUE(fix_utils_atoi64(str3, strlen(str3), 0, &val3) == 6);
   ASSERT_TRUE(val3 == -123);

   char str4[] = "1000=";
   int64_t val4 = 0;
   ASSERT_TRUE(fix_utils_atoi64(str4, strlen(str4), '=', &val4) == 4);
   ASSERT_TRUE(val4 == 1000);

   char str5[] = "1000=";
   int64_t val5 = 0;
   ASSERT_TRUE(fix_utils_atoi64(str5, 3, '=', &val5) == 3);
   ASSERT_TRUE(val5 == 100);

   char str6[] = "1000\00134=";
   int64_t val6 = 0;
   ASSERT_TRUE(fix_utils_atoi64(str6, strlen(str6), '=', &val6) == FIX_FAILED);
   ASSERT_TRUE(val6 == 1000);
}


TEST(FixUtilsTests, atod_Test)
{
   char str[] = "1000.12";
   double val = 0;
   ASSERT_TRUE(fix_utils_atod(str, strlen(str), 0, &val) == 7);
   ASSERT_TRUE(val == 1000.12);

   char str1[] = "-123456.345";
   double val1 = 0;
   ASSERT_TRUE(fix_utils_atod(str1, strlen(str1), 0, &val1) == 11);
   ASSERT_TRUE(val1 == -123456.345);

   char str2[] = "00123.987";
   double val2 = 0;
   ASSERT_TRUE(fix_utils_atod(str2, strlen(str2), 0, &val2) == 9);
   ASSERT_TRUE(val2 == 123.987);

   char str3[] = "-00123.456";
   double val3 = 0;
   ASSERT_TRUE(fix_utils_atod(str3, strlen(str3), 0, &val3) == 10);
   ASSERT_TRUE(val3 == -123.456);

   char str4[] = "123.456\00134=";
   double val4 = 0;
   ASSERT_TRUE(fix_utils_atod(str4, strlen(str4), 1, &val4) == 7);
   ASSERT_TRUE(val4 == 123.456);
}