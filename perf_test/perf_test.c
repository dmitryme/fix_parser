/* @file   perf_test.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/27/2012 05:35:55 PM
*/

#include "fix_parser.h"
#include "fix_msg.h"

#include <stdio.h>
#include <time.h>

void create_msg_test(FIXParser* parser)
{
   struct timespec start, stop;

   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   for(int i = 0; i < 100000; ++i)
   {
      FIXMsg* msg = fix_msg_create(parser, FIX44, "8");
      if (!msg)
      {
         printf("ERROR: %s\n", get_fix_error_text(parser));
         return;
      }

      fix_msg_set_string(msg, NULL, FIXTagNum_SenderCompID, "QWERTY_12345678");
      fix_msg_set_string(msg, NULL, FIXTagNum_TargetCompID, "ABCQWE_XYZ");
      fix_msg_set_int(msg, NULL, FIXTagNum_MsgSeqNum, 34);
      fix_msg_set_string(msg, NULL, FIXTagNum_TargetSubID, "srv-ivanov_ii1");
      fix_msg_set_string(msg, NULL, FIXTagNum_SendingTime, "20120716-06:00:16.230");
      fix_msg_set_string(msg, NULL, FIXTagNum_OrderID, "1");
      fix_msg_set_string(msg, NULL, FIXTagNum_ClOrdID, "CL_ORD_ID_1234567");
      fix_msg_set_string(msg, NULL, FIXTagNum_ExecID, "FE_1_9494_1");
      fix_msg_set_char(msg, NULL, FIXTagNum_ExecType, '0');
      fix_msg_set_char(msg, NULL, FIXTagNum_OrdStatus, '1');
      fix_msg_set_string(msg, NULL, FIXTagNum_Account, "ZUM");
      fix_msg_set_string(msg, NULL, FIXTagNum_Symbol, "RTS-12.12");
      fix_msg_set_char(msg, NULL, FIXTagNum_Side, '1');
      fix_msg_set_float(msg, NULL, FIXTagNum_OrderQty, 25);
      fix_msg_set_float(msg, NULL, FIXTagNum_Price, 135155.0);
      fix_msg_set_char(msg, NULL, FIXTagNum_TimeInForce, '0');
      fix_msg_set_float(msg, NULL, FIXTagNum_LastQty, 0);
      fix_msg_set_float(msg, NULL, FIXTagNum_LastPx, 0.0);
      fix_msg_set_float(msg, NULL, FIXTagNum_LeavesQty, 25.0);
      fix_msg_set_float(msg, NULL, FIXTagNum_CumQty, 0);
      fix_msg_set_float(msg, NULL, FIXTagNum_AvgPx, 0.0);
      fix_msg_set_char(msg, NULL, FIXTagNum_HandlInst, '1');
      fix_msg_set_string(msg, NULL, FIXTagNum_Text, "COMMENT12");

      fix_msg_free(msg);
   }

   clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
   printf("CREATE_MSG %ld\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000);
}

void msg_to_string(FIXParser* parser)
{
   struct timespec start, stop;

   FIXMsg* msg = fix_msg_create(parser, FIX44, "8");
   if (!msg)
   {
      printf("ERROR: %s\n", get_fix_error_text(parser));
      return;
   }

   fix_msg_set_string(msg, NULL, FIXTagNum_SenderCompID, "QWERTY_12345678");
   fix_msg_set_string(msg, NULL, FIXTagNum_TargetCompID, "ABCQWE_XYZ");
   fix_msg_set_int(msg, NULL, FIXTagNum_MsgSeqNum, 34);
   fix_msg_set_string(msg, NULL, FIXTagNum_TargetSubID, "srv-ivanov_ii1");
   fix_msg_set_string(msg, NULL, FIXTagNum_SendingTime, "20120716-06:00:16.230");
   fix_msg_set_string(msg, NULL, FIXTagNum_OrderID, "1");
   fix_msg_set_string(msg, NULL, FIXTagNum_ClOrdID, "CL_ORD_ID_1234567");
   fix_msg_set_string(msg, NULL, FIXTagNum_ExecID, "FE_1_9494_1");
   fix_msg_set_char(msg, NULL, FIXTagNum_ExecType, '0');
   fix_msg_set_char(msg, NULL, FIXTagNum_OrdStatus, '1');
   fix_msg_set_string(msg, NULL, FIXTagNum_Account, "ZUM");
   fix_msg_set_string(msg, NULL, FIXTagNum_Symbol, "RTS-12.12");
   fix_msg_set_char(msg, NULL, FIXTagNum_Side, '1');
   fix_msg_set_float(msg, NULL, FIXTagNum_OrderQty, 25);
   fix_msg_set_float(msg, NULL, FIXTagNum_Price, 135155.0);
   fix_msg_set_char(msg, NULL, FIXTagNum_TimeInForce, '0');
   fix_msg_set_float(msg, NULL, FIXTagNum_LastQty, 0);
   fix_msg_set_float(msg, NULL, FIXTagNum_LastPx, 0.0);
   fix_msg_set_float(msg, NULL, FIXTagNum_LeavesQty, 25.0);
   fix_msg_set_float(msg, NULL, FIXTagNum_CumQty, 0);
   fix_msg_set_float(msg, NULL, FIXTagNum_AvgPx, 0.0);
   fix_msg_set_char(msg, NULL, FIXTagNum_HandlInst, '1');
   fix_msg_set_string(msg, NULL, FIXTagNum_Text, "COMMENT12");

   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   for(int i = 0; i < 100000; ++i)
   {
      char buff[1024];
      fix_msg_to_string(msg, 1, buff, sizeof(buff));
   }

   clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

   fix_msg_free(msg);

   printf("MSG_TO_STR %ld\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000);
}

int main()
{
   FIXParser* parser = fix_parser_create(1024, 0, 2, 0, 1000, 0, 0/*FIXParserFlags_Validate*/);

   int res = fix_protocol_init(parser, "fix44.xml");
   if (res == FIX_FAILED)
   {
      printf("ERROR: %s\n", get_fix_error_text(parser));
      return 1;
   }

   create_msg_test(parser);
   msg_to_string(parser);

   return 0;
}