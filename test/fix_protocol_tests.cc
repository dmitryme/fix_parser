/**
 * @file   fix_protocol_tests.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 10/01/2012 18:20:02 PM
 */

#include  <fix_parser.h>
#include  <fix_parser_priv.h>
#include  <fix_protocol_descr.h>
#include  <fix_field_tag.h>

#include  <gtest/gtest.h>

TEST(FIXProtocolTests, FIXProtocolTest1)
{
   FIXError* error = NULL;
   FIXParser* p = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(p != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "8", &error) != NULL);
   ASSERT_TRUE(error == NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "0", &error) != NULL);
   ASSERT_TRUE(error == NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "1", &error) != NULL);
   ASSERT_TRUE(error == NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "A", &error) != NULL);
   ASSERT_TRUE(error == NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "D", &error) != NULL);
   ASSERT_TRUE(error == NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "AE", &error) != NULL);
   ASSERT_TRUE(error == NULL);

   ASSERT_STREQ("FIX.4.4", p->protocol->version);
   ASSERT_STREQ("FIX.4.4", p->protocol->transportVersion);

   FIXMsgDescr const* msg = fix_protocol_get_msg_descr(p, "8", &error);
   ASSERT_TRUE(msg != NULL);
   ASSERT_STREQ(msg->type, "8");
   ASSERT_STREQ(msg->name, "ExecutionReport");
   ASSERT_EQ(msg->field_count, 247U);
   ASSERT_TRUE(error == NULL);

   FIXFieldDescr const* field = fix_protocol_get_field_descr(msg, FIXFieldTag_BeginString);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_BeginString);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "BeginString");
   ASSERT_TRUE(field->dataLenField == NULL);

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_BodyLength);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_BodyLength);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_Length);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "BodyLength");
   ASSERT_TRUE(field->dataLenField == NULL);

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_MsgType);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_MsgType);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "MsgType");
   ASSERT_TRUE(field->dataLenField == NULL);

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_OrderID);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_OrderID);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "OrderID");
   ASSERT_TRUE(field->dataLenField == NULL);

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_ClOrdID);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_ClOrdID);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "ClOrdID");
   ASSERT_TRUE(field->dataLenField == NULL);

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_SignatureLength);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_SignatureLength);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_Length);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "SignatureLength");
   ASSERT_TRUE(field->dataLenField == NULL);

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_CheckSum);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_CheckSum);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "CheckSum");
   ASSERT_TRUE(field->dataLenField == NULL);

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_OrdRejReason);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(1, fix_protocol_check_field_value(field, "10", 2));
   ASSERT_EQ(0, fix_protocol_check_field_value(field, "100", 3));

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_SecurityType);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(0, fix_protocol_check_field_value(field, "FAKE", 4));
   ASSERT_EQ(1, fix_protocol_check_field_value(field, "DEFLTED", 7));
   ASSERT_EQ(1, fix_protocol_check_field_value(field, "YANK", 4));

   fix_parser_free(p);
}

TEST(FIXProtocolTests, FIXProtocolTest2)
{
   FIXError* error = NULL;
   FIXParser* p = fix_parser_create("fix_descr/fix.4.4.xml", NULL, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(p != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "A", &error) != NULL);
   ASSERT_TRUE(error == NULL);

   ASSERT_STREQ("FIX.4.4", p->protocol->version);
   ASSERT_STREQ("FIX.4.4", p->protocol->transportVersion);

   FIXMsgDescr const* msg = fix_protocol_get_msg_descr(p, "A", &error);
   ASSERT_TRUE(msg != NULL);
   ASSERT_STREQ(msg->type, "A");
   ASSERT_STREQ(msg->name, "Logon");
   ASSERT_EQ(msg->field_count, 41U);

   FIXFieldDescr const* field = fix_protocol_get_field_descr(msg, FIXFieldTag_RawData);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_RawData);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_Data);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "RawData");

   FIXFieldDescr const* lengthField = field->dataLenField;
   ASSERT_TRUE(lengthField != NULL);
   ASSERT_EQ(lengthField->type->tag, FIXFieldTag_RawDataLength);
   ASSERT_EQ(lengthField->type->valueType, FIXFieldValueType_Length);
   ASSERT_EQ(lengthField->category, FIXFieldCategory_Value);
   ASSERT_STREQ(lengthField->type->name, "RawDataLength");
}

TEST(FIXProtocolTests, FIXProtocolTest3)
{
   FIXError* error = NULL;
   FIXParser* p = fix_parser_create("./test_data/fix2.xml", NULL, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(p != NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "8", &error) != NULL);
   ASSERT_TRUE(error == NULL);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "0", &error) == NULL);
   ASSERT_TRUE(error != NULL);
   ASSERT_EQ(error->code, FIX_ERROR_UNKNOWN_MSG);
   fix_error_free(error);
   ASSERT_TRUE(fix_protocol_get_msg_descr(p, "1", &error) == NULL);
   ASSERT_TRUE(error != NULL);
   ASSERT_EQ(error->code, FIX_ERROR_UNKNOWN_MSG);
   fix_error_free(error);

   ASSERT_STREQ("FIX2", p->protocol->version);
   ASSERT_STREQ("FIX2", p->protocol->transportVersion);

   FIXMsgDescr const* msg = fix_protocol_get_msg_descr(p, "8", &error);
   ASSERT_TRUE(msg != NULL);
   ASSERT_STREQ(msg->type, "8");
   ASSERT_STREQ(msg->name, "ExecutionReport");
   ASSERT_EQ(msg->field_count, 11U);

   FIXFieldDescr const* field = fix_protocol_get_field_descr(msg, FIXFieldTag_BeginString);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_BeginString);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "BeginString");

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_BodyLength);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_BodyLength);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_Length);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "BodyLength");

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_MsgType);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_MsgType);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "MsgType");

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_OrderID);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_OrderID);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "OrderID");

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_ClOrdID);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_ClOrdID);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "ClOrdID");

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_SignatureLength);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_SignatureLength);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_Length);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "SignatureLength");

   field = fix_protocol_get_field_descr(msg, FIXFieldTag_CheckSum);
   ASSERT_TRUE(field != NULL);
   ASSERT_EQ(field->type->tag, FIXFieldTag_CheckSum);
   ASSERT_EQ(field->type->valueType, FIXFieldValueType_String);
   ASSERT_EQ(field->category, FIXFieldCategory_Value);
   ASSERT_STREQ(field->type->name, "CheckSum");

   fix_parser_free(p);
}
