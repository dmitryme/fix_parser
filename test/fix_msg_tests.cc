/* @file   fix_msg_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/27/2012 05:09:24 PM
*/

#include <fix_msg.h>
#include <fix_parser.h>
#include <fix_msg_priv.h>
#include <fix_parser_priv.h>

#include <gtest/gtest.h>

TEST(FixMsgTests, CreateMsgTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* p = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(p != NULL);
   FIXGroup* group = p->group;

   FIXMsg* msg = fix_msg_create(p, "8");
   ASSERT_TRUE(msg != NULL);
   ASSERT_EQ(msg->used_groups, group);
   ASSERT_EQ(msg->body_len, 5U);

   char const* buff = NULL;
   uint32_t len = 0;
   ASSERT_EQ(fix_msg_get_string(msg, NULL, 8, &buff, &len), FIX_SUCCESS);
   ASSERT_TRUE(!strncmp(buff, "FIX.4.4", len));

   char const* msgType = NULL;
   ASSERT_EQ(fix_msg_get_string(msg, NULL, 35, &msgType, &len), FIX_SUCCESS);
   ASSERT_TRUE(!strncmp(msgType, "8", len));

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14);

   ASSERT_EQ(fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_TargetSubID, "srv-ivanov_ii1"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_OrderID, "1"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_ExecID, "FE_1_9494_1"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15);

   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_ExecType, '0'), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6);

   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_OrdStatus, '1'), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_Account, "ZUM"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13);

   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1'), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5);

   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6);

   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_Price, 135155.0), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10);

   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_TimeInForce, '0'), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5);

   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_LastQty, 0), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5);

   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_LastPx, 0.0), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5);

   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_LeavesQty, 25.0), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7);

   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_CumQty, 0), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5);

   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_AvgPx, 0.0), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4);

   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_HandlInst, '1'), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_Text, "COMMENT12"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);


   int32_t val = 0;
   ASSERT_EQ(fix_msg_get_int32(msg, NULL, FIXFieldTag_MsgSeqNum, &val), FIX_SUCCESS);
   ASSERT_EQ(val, 34);

   double price = 0.0;
   ASSERT_EQ(fix_msg_get_double(msg, NULL, FIXFieldTag_Price, &price), FIX_SUCCESS);
   ASSERT_EQ(price, 135155.0);

   char const* text = NULL;
   ASSERT_EQ(fix_msg_get_string(msg, NULL, FIXFieldTag_Text, &text, &len), FIX_SUCCESS);
   ASSERT_TRUE(!strncmp(text, "COMMENT12", len));

   ASSERT_EQ(fix_msg_del_field(msg, NULL, FIXFieldTag_Symbol), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);

   ASSERT_EQ(fix_msg_del_field(msg, NULL, FIXFieldTag_Account), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);

   ASSERT_EQ(fix_msg_del_field(msg, NULL, FIXFieldTag_SenderCompID), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_Text, "COMM"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 8);

   fix_msg_free(msg);
   fix_parser_free(p);
}

TEST(FixMsgTests, CreateMsg2Test)
{
   FIXParser* p = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(p != NULL);
   FIXGroup* group = p->group;

   FIXMsg* msg = fix_msg_create(p, "D");
   ASSERT_TRUE(msg != NULL);
   ASSERT_EQ(msg->used_groups, group);
   ASSERT_EQ(msg->body_len, 5U);

   char buff[1024];
   uint32_t reqBuffLen = 0;
   ASSERT_EQ(FIX_FAILED, fix_msg_to_fix(msg, '|', buff, sizeof(buff), &reqBuffLen));

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14);
   ASSERT_EQ(fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13);
   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1'), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_TransactTime, "20120716-06:00:16.230"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25);
   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6);
   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_OrdType, '2'), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5);

   FIXGroup* grp1 = fix_msg_add_group(msg, NULL, FIXFieldTag_NoPartyIDs);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6);
   ASSERT_EQ(fix_msg_set_string(msg, grp1, FIXFieldTag_PartyID, "ID1"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8);
   ASSERT_EQ(fix_msg_set_char(msg, grp1, FIXFieldTag_PartyIDSource, 'A'), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6);
   ASSERT_EQ(fix_msg_set_int32(msg, grp1, FIXFieldTag_PartyRole, 1), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6);

   FIXGroup* grp2 = fix_msg_add_group(msg, NULL, FIXFieldTag_NoPartyIDs);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6);
   ASSERT_EQ(fix_msg_set_string(msg, grp2, FIXFieldTag_PartyID, "ID2"), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6 + 8);
   ASSERT_EQ(fix_msg_set_char(msg, grp2, FIXFieldTag_PartyIDSource, 'B'), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6 + 8 + 6);
   ASSERT_EQ(fix_msg_set_int32(msg, grp2, FIXFieldTag_PartyRole, 2), FIX_SUCCESS);
   ASSERT_EQ(msg->body_len, 5U + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6 + 8 + 6 + 6);
}

TEST(FixMsgTests, ToStringTest)
{
   FIXParser* p = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(p != NULL);
   FIXGroup* group = p->group;

   FIXMsg* msg = fix_msg_create(p, "8");
   ASSERT_TRUE(msg != NULL);
   ASSERT_EQ(msg->used_groups, group);
   ASSERT_EQ(msg->body_len, 5U);

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_TargetSubID, "srv-ivanov_ii1"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_OrderID, "1"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_ExecID, "FE_1_9494_1"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_ExecType, '0'), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_OrdStatus, '1'), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_Account, "ZUM"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1'), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_Price, 135155.0), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_TimeInForce, '0'), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_LastQty, 0), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_LastPx, 0.0), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_LeavesQty, 25.0), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_CumQty, 0), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_AvgPx, 0.0), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_HandlInst, '1'), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_Text, "COMMENT12"), FIX_SUCCESS);

   char buff[1024];
   uint32_t reqBuffLen = 0;
   ASSERT_EQ(FIX_SUCCESS, fix_msg_to_fix(msg, 1, buff,sizeof(buff), &reqBuffLen));
   ASSERT_EQ(reqBuffLen, 251U);
   buff[reqBuffLen] = 0;
   ASSERT_STREQ(buff, "8=FIX.4.4\0019=228\00135=8\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00157=srv-ivanov_ii1\00152=20120716-06:00:16.230\001"
            "37=1\00111=CL_ORD_ID_1234567\00117=FE_1_9494_1\001150=0\00139=1\0011=ZUM\00155=RTS-12.12\00154=1\00138=25\00144=135155\00159=0\00132=0\00131=0\001"
            "151=25\00114=0\0016=0\00121=1\00158=COMMENT12\00110=240\001");

   fix_msg_free(msg);
   fix_parser_free(p);
}

TEST(FixMsgTests, ToStringGroupTest)
{
   FIXParser* p = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(p != NULL);
   FIXGroup* group = p->group;

   FIXMsg* msg = fix_msg_create(p, "D");
   ASSERT_TRUE(msg != NULL);
   ASSERT_EQ(msg->used_groups, group);
   ASSERT_EQ(msg->body_len, 5U);

   char buff[1024];
   uint32_t reqBuffLen = 0;
   ASSERT_EQ(FIX_FAILED, fix_msg_to_fix(msg, '|', buff, sizeof(buff), &reqBuffLen));

   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1'), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_string(msg, NULL, FIXFieldTag_TransactTime, "20120716-06:00:16.230"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_char(msg, NULL, FIXFieldTag_OrdType, '2'), FIX_SUCCESS);

   FIXGroup* grp1 = fix_msg_add_group(msg, NULL, FIXFieldTag_NoPartyIDs);
   ASSERT_EQ(fix_msg_set_string(msg, grp1, FIXFieldTag_PartyID, "ID1"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_char(msg, grp1, FIXFieldTag_PartyIDSource, 'A'), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_int32(msg, grp1, FIXFieldTag_PartyRole, 1), FIX_SUCCESS);

   FIXGroup* grp2 = fix_msg_add_group(msg, NULL, FIXFieldTag_NoPartyIDs);
   ASSERT_EQ(fix_msg_set_string(msg, grp2, FIXFieldTag_PartyID, "ID2"), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_char(msg, grp2, FIXFieldTag_PartyIDSource, 'B'), FIX_SUCCESS);
   ASSERT_EQ(fix_msg_set_int32(msg, grp2, FIXFieldTag_PartyRole, 2), FIX_SUCCESS);

   ASSERT_EQ(FIX_SUCCESS, fix_msg_to_fix(msg, 1, buff, sizeof(buff), &reqBuffLen));
   buff[reqBuffLen] = 0;
   ASSERT_EQ(reqBuffLen, 213U);
   ASSERT_STREQ(buff, "8=FIX.4.4\0019=190\00135=D\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00152=20120716-06:00:16.230\001"
            "11=CL_ORD_ID_1234567\001453=2\001448=ID1\001447=A\001452=1\001448=ID2\001447=B\001452=2\00155=RTS-12.12\001"
            "54=1\00160=20120716-06:00:16.230\00138=25\00140=2\00110=088\001");

   ASSERT_EQ(fix_msg_del_group(msg, NULL, FIXFieldTag_NoPartyIDs, 0), FIX_SUCCESS);
   ASSERT_EQ(FIX_SUCCESS, fix_msg_to_fix(msg, 1, buff, sizeof(buff), &reqBuffLen));
   buff[reqBuffLen] = 0;
   ASSERT_EQ(reqBuffLen, 193U);
   ASSERT_STREQ(buff, "8=FIX.4.4\0019=170\00135=D\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00152=20120716-06:00:16.230\001"
            "11=CL_ORD_ID_1234567\001453=1\001448=ID2\001447=B\001452=2\00155=RTS-12.12\00154=1\00160=20120716-06:00:16.230\00138=25\00140=2\00110=145\001");

   ASSERT_EQ(fix_msg_del_group(msg, NULL, FIXFieldTag_NoPartyIDs, 0), FIX_SUCCESS);
   ASSERT_EQ(FIX_SUCCESS, fix_msg_to_fix(msg, 1, buff, sizeof(buff), &reqBuffLen));
   buff[reqBuffLen] = 0;
   ASSERT_EQ(reqBuffLen, 167U);
   ASSERT_STREQ(buff, "8=FIX.4.4\0019=144\00135=D\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00152=20120716-06:00:16.230\001"
            "11=CL_ORD_ID_1234567\00155=RTS-12.12\00154=1\00160=20120716-06:00:16.230\00138=25\00140=2\00110=192\001");
}
