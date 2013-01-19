/**
 * @file   fix_parser_tests.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 08/03/2012 02:41:02 PM
 */

#include <fix_parser.h>
#include <fix_parser_priv.h>
#include <fix_msg.h>

#include <gtest/gtest.h>

TEST(FixParserTests, ParseFieldTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   char buff[] = "8=FIX4.4|35=D|41=QWERTY|21=123";
   char const* begin = NULL;
   char const* end = NULL;
   FIXTagNum num = fix_parser_parse_mandatory_field(parser, buff, strlen(buff), '|', &begin, &end);
   ASSERT_EQ(num, 8);
   ASSERT_EQ(*begin, 'F');
   ASSERT_EQ(*end, '|');

   char buff1[] = "A=FIX4.4|35=D|41=QWERTY|21=123";
   num = fix_parser_parse_mandatory_field(parser, buff1, strlen(buff1), '|', &begin, &end);
   ASSERT_EQ(num, FIX_FAILED);
   ASSERT_EQ(parser->error.code, FIX_ERROR_INVALID_ARGUMENT);
}

TEST(FixParserTests, ParseBeginStringTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   {
      char buff[] = "A12=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }
   {
      char buff[] = "1=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_FIELD);
   }
   {
      char buff[] = "8=FIX.4.5|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_PROTOCOL_VER);
   }
   {
      char buff[] = "8=FIXT.1.1|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_PROTOCOL_VER);
   }
}

TEST(FixParserTests, ParseBodyLengthTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   {
      char buff[] = "8=FIX.4.4|10=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_FIELD);
   }
   {
      char buff[] = "8=FIX.4.4|9=228A|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, 0);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, 0);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, 0);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }
}

TEST(FixParserTests, ParseCheckSumTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|A10=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|11=240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_WRONG_FIELD);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=A240|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_PARSE_MSG);
   }

   {
      char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=210|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_INTEGRITY_CHECK);
   }

   {
      char buff[] = "8=FIX.4.4|9=230|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|4552=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=208|";
      char const* stop = NULL;
      FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), '|', &stop);
      ASSERT_TRUE(msg == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_UNKNOWN_FIELD);
      ASSERT_TRUE(!strcmp(parser->error.text, "Field '4552' not found in description."));
   }
}

#define CHECK_STRING(msg, group, tag, text) \
{ \
   char const* buff = NULL; \
   uint32_t len = 0; \
   ASSERT_EQ(fix_msg_get_string(msg, group, tag, &buff, &len), FIX_SUCCESS); \
   ASSERT_TRUE(!strncmp(text, buff, len)); \
}
#define CHECK_INT32(msg, group, tag, val) \
{ \
   int32_t tmp = 0; \
   ASSERT_EQ(fix_msg_get_int32(msg, group, tag, &tmp), FIX_SUCCESS); \
   ASSERT_EQ(val, tmp); \
}
#define CHECK_CHAR(msg, group, tag, val) \
{ \
   char tmp = 0; \
   ASSERT_EQ(fix_msg_get_char(msg, group, tag, &tmp), FIX_SUCCESS); \
   ASSERT_EQ(tmp, val); \
}
#define CHECK_DOUBLE(msg, group, tag, val) \
{ \
   double tmp = 0; \
   ASSERT_EQ(fix_msg_get_double(msg, group, tag, &tmp), FIX_SUCCESS); \
   ASSERT_EQ(tmp, val); \
}

TEST(FixParserTests, ParseStringTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);
   char buff[] = "8=FIX.4.4\0019=228\00135=8\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00157=srv-ivanov_ii1\00152=20120716-06:00:16.230\00137=1\001"
      "11=CL_ORD_ID_1234567\00117=FE_1_9494_1\001150=0\00139=1\0011=ZUM\00155=RTS-12.12\00154=1\00138=25\00144=135155\00159=0\00132=0\00131=0\001151=25\001"
      "14=0\0016=0\00121=1\00158=COMMENT12\00110=240\001";
   char const* stop = NULL;
   FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), FIX_SOH, &stop);
   ASSERT_TRUE(msg != NULL);

   CHECK_STRING(msg, NULL, FIXFieldTag_BeginString,  "FIX.4.4");
   CHECK_INT32(msg,  NULL, FIXFieldTag_BodyLength,   228);
   CHECK_STRING(msg, NULL, FIXFieldTag_MsgType,      "8");
   CHECK_STRING(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678");
   CHECK_STRING(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ");
   CHECK_INT32(msg,  NULL, FIXFieldTag_MsgSeqNum,    34);
   CHECK_STRING(msg, NULL, FIXFieldTag_TargetSubID,  "srv-ivanov_ii1");
   CHECK_STRING(msg, NULL, FIXFieldTag_SendingTime,  "20120716-06:00:16.230");
   CHECK_INT32(msg,  NULL, FIXFieldTag_OrderID,      1);
   CHECK_STRING(msg, NULL, FIXFieldTag_ClOrdID,      "CL_ORD_ID_1234567");
   CHECK_STRING(msg, NULL, FIXFieldTag_ExecID,       "FE_1_9494_1");
   CHECK_CHAR(msg,   NULL, FIXFieldTag_ExecType,     '0');
   CHECK_CHAR(msg,   NULL, FIXFieldTag_OrdStatus,    '1');
   CHECK_STRING(msg, NULL, FIXFieldTag_Account,      "ZUM");
   CHECK_STRING(msg, NULL, FIXFieldTag_Symbol,       "RTS-12.12");
   CHECK_CHAR(msg,   NULL, FIXFieldTag_Side,         '1');
   CHECK_DOUBLE(msg, NULL, FIXFieldTag_OrderQty,     25);
   CHECK_DOUBLE(msg, NULL, FIXFieldTag_Price,        135155);
   CHECK_CHAR(msg,   NULL, FIXFieldTag_TimeInForce,  '0');
   CHECK_DOUBLE(msg, NULL, FIXFieldTag_LastQty,      0);
   CHECK_DOUBLE(msg, NULL, FIXFieldTag_LastPx,       0);
   CHECK_DOUBLE(msg, NULL, FIXFieldTag_LeavesQty,    25);
   CHECK_DOUBLE(msg, NULL, FIXFieldTag_CumQty,       0);
   CHECK_DOUBLE(msg, NULL, FIXFieldTag_AvgPx,        0);
   CHECK_STRING(msg, NULL, FIXFieldTag_HandlInst,    "1");
   CHECK_STRING(msg, NULL, FIXFieldTag_Text,         "COMMENT12");
   CHECK_STRING(msg, NULL, FIXFieldTag_CheckSum,     "240");

   char buff1[1024];
   uint32_t reqBuffLen = 0;
   ASSERT_EQ(FIX_SUCCESS, fix_msg_to_str(msg, FIX_SOH, buff1, sizeof(buff1), &reqBuffLen));
   buff1[reqBuffLen] = 0;
   ASSERT_STREQ(buff, buff1); // Bingo!
}

TEST(FixParserTests, ParseStringGroupTest)
{
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);
   char buff[] = "8=FIX.4.4\0019=190\00135=D\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00152=20120716-06:00:16.230\001"
            "11=CL_ORD_ID_1234567\001453=2\001448=ID1\001447=A\001452=1\001448=ID2\001447=B\001452=2\00155=RTS-12.12\001"
            "54=1\00160=20120716-06:00:16.230\00138=25\00140=2\00110=088\001";
   char const* stop = NULL;
   FIXMsg* msg = fix_parser_str_to_msg(parser, buff, strlen(buff), FIX_SOH, &stop);
   ASSERT_TRUE(msg != NULL);

   CHECK_STRING(msg, NULL, FIXFieldTag_BeginString,  "FIX.4.4");
   CHECK_INT32(msg,  NULL, FIXFieldTag_BodyLength,   190);
   CHECK_STRING(msg, NULL, FIXFieldTag_MsgType,      "D");
   CHECK_STRING(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678");
   CHECK_STRING(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ");
   CHECK_INT32(msg,  NULL, FIXFieldTag_MsgSeqNum,    34);
   CHECK_STRING(msg, NULL, FIXFieldTag_SendingTime,  "20120716-06:00:16.230");
   CHECK_STRING(msg, NULL, FIXFieldTag_ClOrdID,      "CL_ORD_ID_1234567");
   CHECK_STRING(msg, NULL, FIXFieldTag_Symbol,       "RTS-12.12");
   CHECK_CHAR(msg,   NULL, FIXFieldTag_Side,         '1');
   CHECK_DOUBLE(msg, NULL, FIXFieldTag_OrderQty,     25);
   CHECK_STRING(msg, NULL, FIXFieldTag_CheckSum,     "088");

   FIXGroup* group = fix_msg_get_group(msg, NULL, FIXFieldTag_NoPartyIDs, 0);
   ASSERT_TRUE(group != NULL);
   CHECK_STRING(msg, group, FIXFieldTag_PartyID, "ID1");
   CHECK_CHAR(msg, group, FIXFieldTag_PartyIDSource, 'A');
   CHECK_INT32(msg, group, FIXFieldTag_PartyRole, 1);

   group = fix_msg_get_group(msg, NULL, FIXFieldTag_NoPartyIDs, 1);
   ASSERT_TRUE(group != NULL);
   CHECK_STRING(msg, group, FIXFieldTag_PartyID, "ID2");
   CHECK_CHAR(msg, group, FIXFieldTag_PartyIDSource, 'B');
   CHECK_INT32(msg, group, FIXFieldTag_PartyRole, 2);
}
