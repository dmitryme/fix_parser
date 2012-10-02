/* @file   perf_test.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/27/2012 05:35:55 PM
*/

#include "fix_parser.h"
#include "fix_msg.h"

#include <stdio.h>
#include <time.h>
#include  <string.h>

void create_msg(FIXParser* parser)
{
   struct timespec start, stop;

   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   for(int i = 0; i < 100000; ++i)
   {
      FIXMsg* msg = fix_msg_create(parser, "8");
      if (!msg)
      {
         printf("ERROR: %s\n", get_fix_error_text(parser));
         return;
      }

      fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678");
      fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ");
      fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34);
      fix_msg_set_string(msg, NULL, FIXFieldTag_TargetSubID, "srv-ivanov_ii1");
      fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230");
      fix_msg_set_string(msg, NULL, FIXFieldTag_OrderID, "1");
      fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567");
      fix_msg_set_string(msg, NULL, FIXFieldTag_ExecID, "FE_1_9494_1");
      fix_msg_set_char(msg, NULL, FIXFieldTag_ExecType, '0');
      fix_msg_set_char(msg, NULL, FIXFieldTag_OrdStatus, '1');
      fix_msg_set_string(msg, NULL, FIXFieldTag_Account, "ZUM");
      fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12");
      fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1');
      fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25);
      fix_msg_set_double(msg, NULL, FIXFieldTag_Price, 135155.0);
      fix_msg_set_char(msg, NULL, FIXFieldTag_TimeInForce, '0');
      fix_msg_set_double(msg, NULL, FIXFieldTag_LastQty, 0);
      fix_msg_set_double(msg, NULL, FIXFieldTag_LastPx, 0.0);
      fix_msg_set_double(msg, NULL, FIXFieldTag_LeavesQty, 25.0);
      fix_msg_set_double(msg, NULL, FIXFieldTag_CumQty, 0);
      fix_msg_set_double(msg, NULL, FIXFieldTag_AvgPx, 0.0);
      fix_msg_set_char(msg, NULL, FIXFieldTag_HandlInst, '1');
      fix_msg_set_string(msg, NULL, FIXFieldTag_Text, "COMMENT12");

      fix_msg_free(msg);
   }

   clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
   printf("CREATE_MSG %ld\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000);
}

void msg_to_fix(FIXParser* parser)
{
   struct timespec start, stop;

   FIXMsg* msg = fix_msg_create(parser, "8");
   if (!msg)
   {
      printf("ERROR: %s\n", get_fix_error_text(parser));
      return;
   }

   fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678");
   fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ");
   fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34);
   fix_msg_set_string(msg, NULL, FIXFieldTag_TargetSubID, "srv-ivanov_ii1");
   fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230");
   fix_msg_set_string(msg, NULL, FIXFieldTag_OrderID, "1");
   fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567");
   fix_msg_set_string(msg, NULL, FIXFieldTag_ExecID, "FE_1_9494_1");
   fix_msg_set_char(msg, NULL, FIXFieldTag_ExecType, '0');
   fix_msg_set_char(msg, NULL, FIXFieldTag_OrdStatus, '1');
   fix_msg_set_string(msg, NULL, FIXFieldTag_Account, "ZUM");
   fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12");
   fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1');
   fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25);
   fix_msg_set_double(msg, NULL, FIXFieldTag_Price, 135155.0);
   fix_msg_set_char(msg, NULL, FIXFieldTag_TimeInForce, '0');
   fix_msg_set_double(msg, NULL, FIXFieldTag_LastQty, 0);
   fix_msg_set_double(msg, NULL, FIXFieldTag_LastPx, 0.0);
   fix_msg_set_double(msg, NULL, FIXFieldTag_LeavesQty, 25.0);
   fix_msg_set_double(msg, NULL, FIXFieldTag_CumQty, 0);
   fix_msg_set_double(msg, NULL, FIXFieldTag_AvgPx, 0.0);
   fix_msg_set_char(msg, NULL, FIXFieldTag_HandlInst, '1');
   fix_msg_set_string(msg, NULL, FIXFieldTag_Text, "COMMENT12");

   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   for(int i = 0; i < 100000; ++i)
   {
      char buff[1024];
      fix_msg_to_string(msg, '|', buff, sizeof(buff));
   }

   clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

   fix_msg_free(msg);

   printf("MSG_TO_FIX %ld\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000);
}

void fix_to_msg(FIXParser* parser)
{
   struct timespec start, stop;

   char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=240|";

   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   FIXMsg* msg = NULL;
   for(int i = 0; i < 100000; ++i)
   {
      msg = parse_fix(parser, buff, strlen(buff), '|');
      fix_msg_free(msg);
   }

   clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

   printf("FIX_TO_MSG %ld\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000);
}

int main()
{
   FIXParser* parser = fix_parser_create(4096, 0, 2, 0, 1000, 0, PARSER_FLAG_CHECK_ALL);

   int res = fix_protocol_init(parser, "fix.4.4.xml");
   if (res == FIX_FAILED)
   {
      printf("ERROR: %s\n", get_fix_error_text(parser));
      return 1;
   }

   create_msg(parser);
   msg_to_fix(parser);
   fix_to_msg(parser);

   return 0;
}
