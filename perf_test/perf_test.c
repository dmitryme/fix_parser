/* @file   perf_test.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/27/2012 05:35:55 PM
*/

#include "fix_parser.h"
#include "fix_msg.h"
#include "fix_error.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>

void create_msg(FIXParser* parser)
{
   struct timespec start, stop;

   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   int const count = 100000;

   for(int i = 0; i < count; ++i)
   {
      FIXMsg* msg = fix_msg_create(parser, "8");
      if (!msg)
      {
         printf("ERROR: %s\n", get_fix_parser_error_text(parser));
         return;
      }

      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ"));
      assert(FIX_SUCCESS == fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_TargetSubID, "srv-ivanov_ii1"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_OrderID, "1"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_ExecID, "FE_1_9494_1"));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_ExecType, '0'));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_OrdStatus, '1'));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Account, "ZUM"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12"));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1'));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_Price, 135155.0));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_TimeInForce, '0'));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LastQty, 0));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LastPx, 0.0));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LeavesQty, 25.0));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_CumQty, 0));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_AvgPx, 0.0));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_HandlInst, '1'));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Text, "COMMENT12"));

      fix_msg_free(msg);
   }

   clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
   int const total = (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
   printf("%12s%12d%12d%10.2f\n", "create_msg", count, total, (float)total/count);
}

void msg_to_fix(FIXParser* parser)
{
   struct timespec start, stop;

   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   int const count = 100000;

   for(int i = 0; i < count; ++i)
   {
      FIXMsg* msg = fix_msg_create(parser, "8");
      if (!msg)
      {
         printf("ERROR: %s\n", get_fix_parser_error_text(parser));
         return;
      }

      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ"));
      assert(FIX_SUCCESS == fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_TargetSubID, "srv-ivanov_ii1"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_OrderID, "1"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_ExecID, "FE_1_9494_1"));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_ExecType, '0'));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_OrdStatus, '1'));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Account, "ZUM"));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12"));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1'));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_Price, 135155.0));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_TimeInForce, '0'));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LastQty, 0));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LastPx, 0.0));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LeavesQty, 25.0));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_CumQty, 0));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_AvgPx, 0.0));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_HandlInst, '1'));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Text, "COMMENT12"));

      char buff[1024];
      fix_msg_to_string(msg, '|', buff, sizeof(buff));

      fix_msg_free(msg);
   }

   clock_gettime(CLOCK_MONOTONIC_RAW, &stop);


   int const total = (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
   printf("%12s%12d%12d%10.2f\n", "msg_to_fix", count, total, (float)total/count);
}

void fix_to_msg(FIXParser* parser)
{
   struct timespec start, stop;

   char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=110|";
   size_t len = strlen(buff);

   clock_gettime(CLOCK_MONOTONIC_RAW, &start);

   FIXMsg* msg = NULL;

   int const count = 100000;

   for(int i = 0; i < count; ++i)
   {
      char const* stop = NULL;
      msg = parse_fix(parser, buff, len, '|', &stop);
      assert(msg != NULL);
      fix_msg_free(msg);
   }

   clock_gettime(CLOCK_MONOTONIC_RAW, &stop);

   int const total = (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_nsec - start.tv_nsec) / 1000;
   printf("%12s%12d%12d%10.2f\n", "fix_to_msg", count, total, (float)total/count);
}

int main(int argc, char *argv[])
{
   if (argc == 1)
   {
      printf("perf_test <prot_file.xml>\n");
      return 1;
   }

   FIXParser* parser = fix_parser_create(argv[1], NULL, PARSER_FLAG_CHECK_ALL);
   if (!parser)
   {
      printf("ERROR: %s\n", get_fix_error_text(parser));
      return 1;
   }

   printf("%12s%12s%12s%12s", "test", "count", "total", "per msg\n");
   create_msg(parser);
   msg_to_fix(parser);
   fix_to_msg(parser);

   return 0;
}
