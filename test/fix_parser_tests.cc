/* @file   fix_parser_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/03/2012 02:41:02 PM
*/

#include <fix_parser.h>
#include <fix_parser_priv.h>

#include <gtest/gtest.h>

TEST(FixParserTests2, ParseFieldTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   char buff[] = "8=FIX4.4|35=D|41=QWERTY|21=123";
   char const* begin = NULL;
   char const* end = NULL;
   int64_t num = fix_parser_parse_field(parser, buff, strlen(buff), '|', &begin, &end);
   ASSERT_EQ(num, 8);
   ASSERT_EQ(*begin, 'F');
   ASSERT_EQ(*end, '|');

   char buff1[] = "A=FIX4.4|35=D|41=QWERTY|21=123";
   num = fix_parser_parse_field(parser, buff1, strlen(buff1), '|', &begin, &end);
   ASSERT_EQ(num, FIX_FAILED);
   ASSERT_EQ(parser->error.code, FIX_ERROR_INVALID_ARGUMENT);
}

TEST(FixParserTests2, ParseBeginStringTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   {
      char buff[] = "A12=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }
   {
      char buff[] = "1=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_FIELD);
   }
   {
      char buff[] = "8=FIX.4.5|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_PROTOCOL_VER);
   }
   {
      char buff[] = "8=FIXT.1.1|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_PROTOCOL_VER);
   }
}

TEST(FixParserTests2, ParseBodyLengthTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   {
      char buff[] = "8=FIX.4.4|10=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_FIELD);
   }
   {
      char buff[] = "8=FIX.4.4|9=228A|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, 0);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, 0);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, 0);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }
}

TEST(FixParserTests2, ParseCheckSumTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|A10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|11=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_FIELD);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=A240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=210|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_INTEGRITY_CHECK);
   }

   {
      char buff[] = "8=FIX.4.4|9=230|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|4552=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=208|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_fix_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_UNKNOWN_FIELD);
      ASSERT_STREQ(parser->error.text, "Field '4552' not found in description.");
   }
}
