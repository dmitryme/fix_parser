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
   FIXParser* p = fix_parser_create(512, 0, 2, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
   fail_unless(p != NULL);
   FIXGroup* group = p->group;
   fail_unless(fix_protocol_init(p, "fix44.xml") == FIX_SUCCESS);

   FIXMsg* msg = fix_msg_create(p, FIX44, "8");
   fail_unless(msg != NULL);
   fail_unless(msg->used_groups == group);
   fail_unless(msg->body_len == 5);

   char buff[10];
   fail_unless(fix_msg_get_string(msg, NULL, 8, buff, sizeof(buff)) == 7);
   fail_unless(!strcmp(buff, "FIX.4.4"));

   char msgType[10];
   fail_unless(fix_msg_get_string(msg, NULL, 35, msgType, sizeof(msgType)) == 1);
   fail_unless(!strcmp(msgType, "8"));

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SenderCompID, "QWERTY_12345678") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TargetCompID, "ABCQWE_XYZ") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14);

   fail_unless(fix_msg_set_int32(msg, NULL, FIXTagNum_MsgSeqNum, 34) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TargetSubID, "srv-ivanov_ii1") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SendingTime, "20120716-06:00:16.230") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_OrderID, "1") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_ClOrdID, "CL_ORD_ID_1234567") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_ExecID, "FE_1_9494_1") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_ExecType, '0') == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_OrdStatus, '1') == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Account, "ZUM") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Symbol, "RTS-12.12") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_Side, '1') == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_OrderQty, 25) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_Price, 135155.0) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_TimeInForce, '0') == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_LastQty, 0) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_LastPx, 0.0) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_LeavesQty, 25.0) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_CumQty, 0) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_AvgPx, 0.0) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_HandlInst, '1') == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Text, "COMMENT12") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);


   int32_t val = 0;
   fail_unless(fix_msg_get_int32(msg, NULL, FIXTagNum_MsgSeqNum, &val) == FIX_SUCCESS);
   fail_unless(val == 34);

   double price = 0.0;
   fail_unless(fix_msg_get_double(msg, NULL, FIXTagNum_Price, &price) == FIX_SUCCESS);
   fail_unless(price == 135155.0);

   char text[10] = {};
   fail_unless(fix_msg_get_string(msg, NULL, FIXTagNum_Text, text, sizeof(text)) == 9);
   fail_unless(!strncmp(text, "COMMENT12", 9));

   fail_unless(fix_msg_del_tag(msg, NULL, FIXTagNum_Symbol) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);

   fail_unless(fix_msg_del_tag(msg, NULL, FIXTagNum_Account) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);

   fail_unless(fix_msg_del_tag(msg, NULL, FIXTagNum_SenderCompID) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Text, "COMM") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 8);

   fix_msg_free(msg);
   fix_parser_free(p);
}
END_TEST

START_TEST(CreateMsg2Test)
{
   FIXParser* p = fix_parser_create(512, 0, 2, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
   fail_unless(p != NULL);
   FIXGroup* group = p->group;
   fail_unless(fix_protocol_init(p, "fix44.xml") == FIX_SUCCESS);

   FIXMsg* msg = fix_msg_create(p, FIX44, "D");
   fail_unless(msg != NULL);
   fail_unless(msg->used_groups == group);
   fail_unless(msg->body_len == 5);

   char buff[1024];
   int32_t res = fix_msg_to_string(msg, '|', buff, sizeof(buff));
   fail_unless(res == -1);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SenderCompID, "QWERTY_12345678") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TargetCompID, "ABCQWE_XYZ") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14);
   fail_unless(fix_msg_set_int32(msg, NULL, FIXTagNum_MsgSeqNum, 34) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SendingTime, "20120716-06:00:16.230") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_ClOrdID, "CL_ORD_ID_1234567") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Symbol, "RTS-12.12") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13);
   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_Side, '1') == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TransactTime, "20120716-06:00:16.230") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25);
   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_OrderQty, 25) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6);
   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_OrdType, '2') == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5);

   FIXGroup* grp1 = fix_msg_add_group(msg, NULL, FIXTagNum_NoPartyIDs);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6);
   fail_unless(fix_msg_set_string(msg, grp1, FIXTagNum_PartyID, "ID1") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8);
   fail_unless(fix_msg_set_char(msg, grp1, FIXTagNum_PartyIDSource, 'A') == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6);
   fail_unless(fix_msg_set_int32(msg, grp1, FIXTagNum_PartyRole, 1) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6);

   FIXGroup* grp2 = fix_msg_add_group(msg, NULL, FIXTagNum_NoPartyIDs);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6);
   fail_unless(fix_msg_set_string(msg, grp2, FIXTagNum_PartyID, "ID2") == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6 + 8);
   fail_unless(fix_msg_set_char(msg, grp2, FIXTagNum_PartyIDSource, 'B') == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6 + 8 + 6);
   fail_unless(fix_msg_set_int32(msg, grp2, FIXTagNum_PartyRole, 2) == FIX_SUCCESS);
   fail_unless(msg->body_len == 5 + 19 + 14 + 6 + 25 + 21 + 13 + 5 + 25 + 6 + 5 + 6 + 8 + 6 + 6 + 8 + 6 + 6);
}
END_TEST


START_TEST(ToStringTest)
{
   FIXParser* p = fix_parser_create(512, 0, 2, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
   fail_unless(p != NULL);
   FIXGroup* group = p->group;
   fail_unless(fix_protocol_init(p, "fix44.xml") == FIX_SUCCESS);

   FIXMsg* msg = fix_msg_create(p, FIX44, "8");
   fail_unless(msg != NULL);
   fail_unless(msg->used_groups == group);
   fail_unless(msg->body_len == 5);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SenderCompID, "QWERTY_12345678") == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TargetCompID, "ABCQWE_XYZ") == FIX_SUCCESS);
   fail_unless(fix_msg_set_int32(msg, NULL, FIXTagNum_MsgSeqNum, 34) == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TargetSubID, "srv-ivanov_ii1") == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SendingTime, "20120716-06:00:16.230") == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_OrderID, "1") == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_ClOrdID, "CL_ORD_ID_1234567") == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_ExecID, "FE_1_9494_1") == FIX_SUCCESS);
   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_ExecType, '0') == FIX_SUCCESS);
   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_OrdStatus, '1') == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Account, "ZUM") == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Symbol, "RTS-12.12") == FIX_SUCCESS);
   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_Side, '1') == FIX_SUCCESS);
   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_OrderQty, 25) == FIX_SUCCESS);
   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_Price, 135155.0) == FIX_SUCCESS);
   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_TimeInForce, '0') == FIX_SUCCESS);
   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_LastQty, 0) == FIX_SUCCESS);
   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_LastPx, 0.0) == FIX_SUCCESS);
   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_LeavesQty, 25.0) == FIX_SUCCESS);
   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_CumQty, 0) == FIX_SUCCESS);
   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_AvgPx, 0.0) == FIX_SUCCESS);
   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_HandlInst, '1') == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Text, "COMMENT12") == FIX_SUCCESS);

   char buff[1024];
   int32_t res = fix_msg_to_string(msg, 1, buff,sizeof(buff));
   fail_unless(res == 251);
   buff[res] = 0;
   fail_unless(!strcmp(buff, "8=FIX.4.4\0019=228\00135=8\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00157=srv-ivanov_ii1\00152=20120716-06:00:16.230\001"
            "37=1\00111=CL_ORD_ID_1234567\00117=FE_1_9494_1\001150=0\00139=1\0011=ZUM\00155=RTS-12.12\00154=1\00138=25\00144=135155\00159=0\00132=0\00131=0\001"
            "151=25\00114=0\0016=0\00121=1\00158=COMMENT12\00110=240\001"));

   fix_msg_free(msg);
   fix_parser_free(p);
}
END_TEST

START_TEST(ToStringGroupTest)
{
   FIXParser* p = fix_parser_create(512, 0, 2, 0, 2, 0, PARSER_FLAG_CHECK_ALL);
   fail_unless(p != NULL);
   FIXGroup* group = p->group;
   fail_unless(fix_protocol_init(p, "fix44.xml") == FIX_SUCCESS);

   FIXMsg* msg = fix_msg_create(p, FIX44, "D");
   fail_unless(msg != NULL);
   fail_unless(msg->used_groups == group);
   fail_unless(msg->body_len == 5);

   char buff[1024];
   int32_t res = fix_msg_to_string(msg, '|', buff, sizeof(buff));
   fail_unless(res == -1);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SenderCompID, "QWERTY_12345678") == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TargetCompID, "ABCQWE_XYZ") == FIX_SUCCESS);
   fail_unless(fix_msg_set_int32(msg, NULL, FIXTagNum_MsgSeqNum, 34) == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SendingTime, "20120716-06:00:16.230") == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_ClOrdID, "CL_ORD_ID_1234567") == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Symbol, "RTS-12.12") == FIX_SUCCESS);
   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_Side, '1') == FIX_SUCCESS);
   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TransactTime, "20120716-06:00:16.230") == FIX_SUCCESS);
   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_OrderQty, 25) == FIX_SUCCESS);
   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_OrdType, '2') == FIX_SUCCESS);

   FIXGroup* grp1 = fix_msg_add_group(msg, NULL, FIXTagNum_NoPartyIDs);
   fail_unless(fix_msg_set_string(msg, grp1, FIXTagNum_PartyID, "ID1") == FIX_SUCCESS);
   fail_unless(fix_msg_set_char(msg, grp1, FIXTagNum_PartyIDSource, 'A') == FIX_SUCCESS);
   fail_unless(fix_msg_set_int32(msg, grp1, FIXTagNum_PartyRole, 1) == FIX_SUCCESS);

   FIXGroup* grp2 = fix_msg_add_group(msg, NULL, FIXTagNum_NoPartyIDs);
   fail_unless(fix_msg_set_string(msg, grp2, FIXTagNum_PartyID, "ID2") == FIX_SUCCESS);
   fail_unless(fix_msg_set_char(msg, grp2, FIXTagNum_PartyIDSource, 'B') == FIX_SUCCESS);
   fail_unless(fix_msg_set_int32(msg, grp2, FIXTagNum_PartyRole, 2) == FIX_SUCCESS);

   res = fix_msg_to_string(msg, 1, buff, sizeof(buff));
   buff[res] = 0;
   fail_unless(res == 213);
   fail_unless(!strcmp(buff, "8=FIX.4.4\0019=190\00135=D\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00152=20120716-06:00:16.230\001"
            "11=CL_ORD_ID_1234567\001453=2\001448=ID1\001447=A\001452=1\001448=ID2\001447=B\001452=2\00155=RTS-12.12\001"
            "54=1\00160=20120716-06:00:16.230\00138=25\00140=2\00110=088\001"));

   fail_unless(fix_msg_del_group(msg, NULL, FIXTagNum_NoPartyIDs, 0) == FIX_SUCCESS);
   res = fix_msg_to_string(msg, 1, buff, sizeof(buff));
   buff[res] = 0;
   fail_unless(res == 193);
   fail_unless(!strcmp(buff, "8=FIX.4.4\0019=170\00135=D\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00152=20120716-06:00:16.230\001"
            "11=CL_ORD_ID_1234567\001453=1\001448=ID2\001447=B\001452=2\00155=RTS-12.12\00154=1\00160=20120716-06:00:16.230\00138=25\00140=2\00110=145\001"));

   fail_unless(fix_msg_del_group(msg, NULL, FIXTagNum_NoPartyIDs, 0) == FIX_SUCCESS);
   res = fix_msg_to_string(msg, 1, buff, sizeof(buff));
   buff[res] = 0;
   fail_unless(res == 167);
   fail_unless(!strcmp(buff, "8=FIX.4.4\0019=144\00135=D\00149=QWERTY_12345678\00156=ABCQWE_XYZ\00134=34\00152=20120716-06:00:16.230\001"
            "11=CL_ORD_ID_1234567\00155=RTS-12.12\00154=1\00160=20120716-06:00:16.230\00138=25\00140=2\00110=192\001"));
}
END_TEST

Suite* make_fix_msg_tests_suite()
{
   Suite* s = suite_create("fix_msg");
   TCase* tc_core = tcase_create("Core");
   tcase_add_test(tc_core, CreateMsgTest);
   tcase_add_test(tc_core, CreateMsg2Test);
   tcase_add_test(tc_core, ToStringTest);
   tcase_add_test(tc_core, ToStringGroupTest);
   suite_add_tcase(s, tc_core);
   return s;
}