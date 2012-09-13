/* @file   fix_msg_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/27/2012 05:09:24 PM
*/

#include <fix_msg.h>
#include <fix_parser.h>
#include <fix_msg_priv.h>
#include <fix_parser_priv.h>

#include <check.h>

#define HEADER_LENGTH 15

START_TEST(CreateMsgTest)
{
   FIXParser* p = fix_parser_create(512, 0, 2, 0, 2, 0, FIXParserFlag_Validate);
   fail_unless(p != NULL);
   FIXGroup* group = p->group;
   fail_unless(fix_protocol_init(p, "fix44.xml") == FIX_SUCCESS);

   FIXMsg* msg = fix_msg_create(p, FIX44, "8");
   fail_unless(msg != NULL);
   fail_unless(msg->used_groups == group);
   fail_unless(msg->body_len == 15);

   char buff[10];
   fail_unless(fix_msg_get_string(msg, NULL, 8, buff, sizeof(buff)) == 7);
   fail_unless(!strcmp(buff, "FIX.4.4"));

   char msgType[10];
   fail_unless(fix_msg_get_string(msg, NULL, 35, msgType, sizeof(msgType)) == 1);
   fail_unless(!strcmp(msgType, "8"));

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SenderCompID, "QWERTY_12345678") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TargetCompID, "ABCQWE_XYZ") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14);

   fail_unless(fix_msg_set_int32(msg, NULL, FIXTagNum_MsgSeqNum, 34) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_TargetSubID, "srv-ivanov_ii1") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_SendingTime, "20120716-06:00:16.230") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_OrderID, "1") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_ClOrdID, "CL_ORD_ID_1234567") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_ExecID, "FE_1_9494_1") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_ExecType, '0') == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_OrdStatus, '1') == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Account, "ZUM") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Symbol, "RTS-12.12") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_Side, '1') == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_OrderQty, 25) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_Price, 135155.0) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_TimeInForce, '0') == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_LastQty, 0) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_LastPx, 0.0) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_LeavesQty, 25.0) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_CumQty, 0) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5);

   fail_unless(fix_msg_set_double(msg, NULL, FIXTagNum_AvgPx, 0.0) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4);

   fail_unless(fix_msg_set_char(msg, NULL, FIXTagNum_HandlInst, '1') == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Text, "COMMENT12") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 13 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);


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
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 6 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);

   fail_unless(fix_msg_del_tag(msg, NULL, FIXTagNum_Account) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 19 + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);

   fail_unless(fix_msg_del_tag(msg, NULL, FIXTagNum_SenderCompID) == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 13);

   fail_unless(fix_msg_set_string(msg, NULL, FIXTagNum_Text, "COMM") == FIX_SUCCESS);
   fail_unless(msg->body_len == HEADER_LENGTH + 14 + 6 + 18 + 25 + 5 + 21 + 15 + 6 + 5 + 6 + 5 + 10 + 5 + 5 + 5 + 7 + 5 + 4 + 5 + 8);

   fix_msg_free(msg);
   fix_parser_free(p);
}
END_TEST


START_TEST(ToStringTest)
{
   FIXParser* p = fix_parser_create(512, 0, 2, 0, 2, 0, FIXParserFlag_Validate);
   fail_unless(p != NULL);
   FIXGroup* group = p->group;
   fail_unless(fix_protocol_init(p, "fix44.xml") == FIX_SUCCESS);

   FIXMsg* msg = fix_msg_create(p, FIX44, "8");
   fail_unless(msg != NULL);
   fail_unless(msg->used_groups == group);
   fail_unless(msg->body_len == 15);

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
   int32_t res = fix_msg_to_string(msg, '|', buff,sizeof(buff));
   fail_unless(res == 251);
   buff[res] = 0;
   fail_unless(!strcmp(buff, "8=FIX.4.4|9=238|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|"
            "37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=111|"));

   fix_msg_free(msg);
   fix_parser_free(p);
}
END_TEST

Suite* make_fix_msg_tests_suite()
{
   Suite* s = suite_create("fix_msg");
   TCase* tc_core = tcase_create("Core");
   tcase_add_test(tc_core, CreateMsgTest);
   tcase_add_test(tc_core, ToStringTest);
   suite_add_tcase(s, tc_core);
   return s;
}
