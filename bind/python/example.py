from fix_parser import *
from fix_fields import *

p = FixParser("fix.4.4.xml", PARSER_FLAG_CHECK_ALL)
print("PROT VERSION = {0}").format(p.getProtocolVer())
# create Exec Report
m = p.createMsg("8")
# ccheck its params
print("MSG TYPE = {0}").format(m.getType())
print("MSG NAME = {0}").format(m.getName())

# fill message
m.setFieldAsString(FIXFieldTag_SenderCompID, "QWERTY_12345678")
m.setFieldAsString(FIXFieldTag_TargetCompID, "ABCQWE_XYZ")
m.setFieldAsInt32(FIXFieldTag_MsgSeqNum, 34)
m.setFieldAsString(FIXFieldTag_TargetSubID, "srv-ivanov_ii1")
m.setFieldAsString(FIXFieldTag_SendingTime, "20120716-06:00:16.230")
m.setFieldAsString(FIXFieldTag_OrderID, "1")
m.setFieldAsString(FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567")
m.setFieldAsString(FIXFieldTag_ExecID, "FE_1_9494_1")
m.setFieldAsChar(FIXFieldTag_ExecType, "0")
m.setFieldAsChar(FIXFieldTag_OrdStatus, "1")
m.setFieldAsString(FIXFieldTag_Account, "ZUM")
m.setFieldAsString(FIXFieldTag_Symbol, "RTS-12.12")
m.setFieldAsChar(FIXFieldTag_Side, "1")
m.setFieldAsDouble(FIXFieldTag_OrderQty, 25.0)
m.setFieldAsDouble(FIXFieldTag_Price, 135155.0)
m.setFieldAsChar(FIXFieldTag_TimeInForce, "0")
m.setFieldAsDouble(FIXFieldTag_LastQty, 0)
m.setFieldAsDouble(FIXFieldTag_LastPx, 0)
m.setFieldAsDouble(FIXFieldTag_LeavesQty, 25.0)
m.setFieldAsDouble(FIXFieldTag_CumQty, 0)
m.setFieldAsDouble(FIXFieldTag_AvgPx, 0)
m.setFieldAsChar(FIXFieldTag_HandlInst, "1")
m.setFieldAsString(FIXFieldTag_Text, "COMMENT12")

# convert msg to string, \1 delimiter replaced with '|', just for pretty printing. '|' code is 124
str = m.toString(124)
print( "MSG = {0}").format(str)

# parse input string and create FIX message
m1 = p.parse(str, 124)

# just print several fields of m1, to make sure str parsed ok
print("MSG1 TYPE = {0}").format(m1.getType())
print("MSG1 NAME = {0}").format(m1.getName())
print("SenderCompID = {0}").format(m1.getFieldAsString(FIXFieldTag_SenderCompID))
print("TargetCompID = {0}").format(m1.getFieldAsString(FIXFieldTag_TargetCompID))
