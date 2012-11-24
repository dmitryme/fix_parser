/* @file   fix_parser_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/03/2012 02:41:02 PM
*/

#include <check.h>

#include <fix_parser.h>
#include <fix_parser_priv.h>

START_TEST(ParseFieldTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->error.code == 0);

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
   fail_unless(parser->error.code == FIX_ERROR_INVALID_ARGUMENT);
}
END_TEST

START_TEST(ParseBeginStringTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->error.code == 0);

   {
      char buff[] = "A12=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_PARSE_MSG);
   }
   {
      char buff[] = "1=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_WRONG_FIELD);
   }
   {
      char buff[] = "8=FIX.4.5|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_WRONG_PROTOCOL_VER);
   }
   {
      char buff[] = "8=FIXT.1.1|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_WRONG_PROTOCOL_VER);
   }
}
END_TEST

START_TEST(ParseBodyLengthTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->error.code == 0);

   {
      char buff[] = "8=FIX.4.4|10=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_WRONG_FIELD);
   }
   {
      char buff[] = "8=FIX.4.4|9=228A|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_PARSE_MSG);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == 0);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == 0);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == 0);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_PARSE_MSG);
   }
}
END_TEST

START_TEST(ParseCheckSumTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   fail_unless(parser != NULL);
   fail_unless(parser->error.code == 0);

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|A10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_PARSE_MSG);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|11=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_WRONG_FIELD);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=A240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_PARSE_MSG);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=210|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_INTEGRITY_CHECK);
   }

   {
      char buff[] = "8=FIX.4.4|9=230|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|4552=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=208|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      fail_unless(msg == NULL);
      fail_unless(parser->error.code == FIX_ERROR_UNKNOWN_FIELD);
      fail_unless(!strcmp(parser->error.text, "Field '4552' not found in description."));
   }
}
END_TEST

Suite* make_fix_parser_tests2_suite()
{
   Suite* s = suite_create("fix_parser");
   TCase* tc_core = tcase_create("Core");
   tcase_add_test(tc_core, ParseFieldTest);
   tcase_add_test(tc_core, ParseBeginStringTest);
   tcase_add_test(tc_core, ParseBodyLengthTest);
   tcase_add_test(tc_core, ParseCheckSumTest);
   suite_add_tcase(s, tc_core);
   return s;
}