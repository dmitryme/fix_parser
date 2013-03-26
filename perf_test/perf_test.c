/* @file   perf_test.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/27/2012 05:35:55 PM
*/

#include "fix_parser.h"
#include "fix_msg.h"
#include "fix_error.h"

#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#  include <windows.h>
#else
#  include <time.h>
#endif
#include <string.h>
#include <assert.h>

#ifdef WIN32
#  define TIMESTAMP LARGE_INTEGER
#  define TIMESTAMP_INIT \
   LARGE_INTEGER freq_1977_04_23; \
   QueryPerformanceFrequency(&freq_1977_04_23);
#  define GET_TIMESTAMP(ts) QueryPerformanceCounter(&ts);
#  define GET_TIMESTAMP_DIFF_USEC(ts1, ts2) \
   (ts1.QuadPart - ts2.QuadPart) * 1000000 / freq_1977_04_23.QuadPart
#else
#  define TIMESTAMP struct timespec
#  define TIMESTAMP_INIT
#  define GET_TIMESTAMP(ts) \
   clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#define GET_TIMESTAMP_DIFF_USEC(ts1, ts2) \
   (ts1.tv_sec - ts2.tv_sec) * 1000000 + (ts1.tv_nsec - ts2.tv_nsec) / 1000;
#endif

void create_msg(FIXParser* parser)
{
   TIMESTAMP_INIT;
   TIMESTAMP start, stop;

   GET_TIMESTAMP(start);

   int32_t const count = 100000;

   for(int32_t i = 0; i < count; ++i)
   {
      FIXError* error = NULL;
      FIXMsg* msg = fix_msg_create(parser, "8", &error);
      if (!msg)
      {
         printf("ERROR: %s\n", fix_error_get_text(error));
         fix_error_free(error);
         return;
      }

      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ", &error));
      assert(FIX_SUCCESS == fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34, &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_TargetSubID, "srv-ivanov_ii1", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_OrderID, "1", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_ExecID, "FE_1_9494_1", &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_ExecType, '0', &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_OrdStatus, '1', &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Account, "ZUM", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12", &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1', &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_Price, 135155.0, &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_TimeInForce, '0', &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LastQty, 0, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LastPx, 0.0, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LeavesQty, 25.0, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_CumQty, 0, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_AvgPx, 0.0, &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_HandlInst, '1', &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Text, "COMMENT12", &error));

      fix_msg_free(msg);
   }

   GET_TIMESTAMP(stop);
   int32_t const total = GET_TIMESTAMP_DIFF_USEC(stop, start);
   printf("%12s%12d%12d%10.2f\n", "create_msg", count, total, (float)total/count);
}

void msg_to_str(FIXParser* parser)
{
   TIMESTAMP_INIT;
   TIMESTAMP start, stop;

   GET_TIMESTAMP(start);

   int32_t const count = 100000;

   for(int32_t i = 0; i < count; ++i)
   {
      FIXError* error = NULL;
      FIXMsg* msg = fix_msg_create(parser, "8", &error);
      if (!msg)
      {
         printf("ERROR: %s\n", fix_error_get_text(error));
         fix_error_free(error);
         return;
      }

      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_SenderCompID, "QWERTY_12345678", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_TargetCompID, "ABCQWE_XYZ", &error));
      assert(FIX_SUCCESS == fix_msg_set_int32(msg, NULL, FIXFieldTag_MsgSeqNum, 34, &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_TargetSubID, "srv-ivanov_ii1", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_SendingTime, "20120716-06:00:16.230", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_OrderID, "1", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_ExecID, "FE_1_9494_1", &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_ExecType, '0', &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_OrdStatus, '1', &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Account, "ZUM", &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Symbol, "RTS-12.12", &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_Side, '1', &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_OrderQty, 25, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_Price, 135155.0, &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_TimeInForce, '0', &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LastQty, 0, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LastPx, 0.0, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_LeavesQty, 25.0, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_CumQty, 0, &error));
      assert(FIX_SUCCESS == fix_msg_set_double(msg, NULL, FIXFieldTag_AvgPx, 0.0, &error));
      assert(FIX_SUCCESS == fix_msg_set_char(msg, NULL, FIXFieldTag_HandlInst, '1', &error));
      assert(FIX_SUCCESS == fix_msg_set_string(msg, NULL, FIXFieldTag_Text, "COMMENT12", &error));

      char buff[1024];
      uint32_t reqBuffLen = 0;
      fix_msg_to_str(msg, '|', buff, sizeof(buff), &reqBuffLen, &error);

      fix_msg_free(msg);
   }

   GET_TIMESTAMP(stop);

   int32_t const total = GET_TIMESTAMP_DIFF_USEC(stop, start);
   printf("%12s%12d%12d%10.2f\n", "msg_to_str", count, total, (float)total/count);
}

void str_to_msg(FIXParser* parser)
{
   TIMESTAMP_INIT;
   TIMESTAMP start, stop;

   char buff[] = "8=FIX.4.4|9=228|35=8|49=QWERTY_12345678|56=ABCQWE_XYZ|34=34|57=srv-ivanov_ii1|52=20120716-06:00:16.230|37=1|11=CL_ORD_ID_1234567|17=FE_1_9494_1|150=0|39=1|1=ZUM|55=RTS-12.12|54=1|38=25|44=135155|59=0|32=0|31=0|151=25|14=0|6=0|21=1|58=COMMENT12|10=110|";
   size_t len = strlen(buff);

   GET_TIMESTAMP(start);

   FIXMsg* msg = NULL;

   int32_t const count = 100000;

   for(int32_t i = 0; i < count; ++i)
   {
      FIXError* error = NULL;
      char const* stop = NULL;
      msg = fix_parser_str_to_msg(parser, buff, len, '|', &stop, &error);
      assert(msg != NULL);
      fix_msg_free(msg);
   }

   GET_TIMESTAMP(stop);

   int32_t const total = GET_TIMESTAMP_DIFF_USEC(stop, start);
   printf("%12s%12d%12d%10.2f\n", "str_to_msg", count, total, (float)total/count);
}

int main(int argc, char *argv[])
{
   if (argc == 1)
   {
      printf("perf_test <prot_file.xml>\n");
      return 1;
   }

   FIXError* error = NULL;
   FIXParser* parser = fix_parser_create(argv[1], NULL, PARSER_FLAG_CHECK_ALL, &error);
   if (!parser)
   {
      printf("ERROR: %s\n", fix_error_get_text(error));
      free(error);
      return 1;
   }

   printf("%12s%12s%12s%12s", "test", "count", "total", "per msg\n");
   create_msg(parser);
   msg_to_str(parser);
   str_to_msg(parser);

   fix_parser_free(parser);

   return 0;
}
