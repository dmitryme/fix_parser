/* @file   fix_protocol_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 10/01/2012 18:20:02 PM
*/

#include  <fix_parser.h>
#include  <fix_parser_priv.h>
#include  <fix_protocol_descr.h>
#include  <fix_field_tag.h>

#include  <gtest/gtest.h>

TEST(FIXProtocolTests, FIXProtocolTest1)
{
   FIXParser* p = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(p != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "8") != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "0") != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "1") != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "A") != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "D") != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "AE") != NULL);

   ASSERT_TRUE(!strcmp("FIX.4.4", p->protocol->version));
   ASSERT_TRUE(!strcmp("FIX.4.4", p->protocol->transportVersion));

   FIXMsgDescr* msg = fix_protocol_get_msg_descr(p, "8");
   ASSERT_TRUE(msg != NULL);
   ASSERT_TRUE(!strcmp(msg->type, "8"));
   ASSERT_TRUE(!strcmp(msg->name, "ExecutionReport"));
   ASSERT_EQ(msg->field_count, 247U);

   FIXFieldDescr* field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_BeginString);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_BeginString);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "BeginString"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_BodyLength);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_BodyLength);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_Length);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "BodyLength"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_MsgType);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_MsgType);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "MsgType"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_OrderID);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_OrderID);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "OrderID"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_ClOrdID);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_ClOrdID);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "ClOrdID"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_SignatureLength);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_SignatureLength);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_Length);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "SignatureLength"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_CheckSum);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_CheckSum);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "CheckSum"));

   fix_parser_free(p);
}

TEST(FIXProtocolTests, FIXProtocolTest2)
{
   FIXParser* p = fix_parser_create("./data/fix2.xml", NULL, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(p != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "8") != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "0") == NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "1") == NULL);

   ASSERT_TRUE(!strcmp("FIX2", p->protocol->version));
   ASSERT_TRUE(!strcmp("FIX2", p->protocol->transportVersion));

   FIXMsgDescr* msg = fix_protocol_get_msg_descr(p, "8");
   ASSERT_TRUE(msg != NULL);
   ASSERT_TRUE(!strcmp(msg->type, "8"));
   ASSERT_TRUE(!strcmp(msg->name, "ExecutionReport"));
   ASSERT_EQ(msg->field_count, 11U);

   FIXFieldDescr* field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_BeginString);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_BeginString);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "BeginString"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_BodyLength);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_BodyLength);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_Length);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "BodyLength"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_MsgType);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_MsgType);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "MsgType"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_OrderID);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_OrderID);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "OrderID"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_ClOrdID);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_ClOrdID);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "ClOrdID"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_SignatureLength);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_SignatureLength);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_Length);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "SignatureLength"));

   field = fix_protocol_get_field_descr(&p->error, msg, FIXFieldTag_CheckSum);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_CheckSum);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_TRUE(!strcmp(field->type->name, "CheckSum"));

   fix_parser_free(p);
}