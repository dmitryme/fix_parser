/* @file   fix_types.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/01/2012 01:53:57 PM
*/

#include "fix_types.h"

#include "string.h"

FIXProtocolVerEnum str2FIXProtocolVerEnum(char const* ver)
{
   if (!strcmp(ver, "FIX42")) return FIX42;
   if (!strcmp(ver, "FIX44")) return FIX44;
   if (!strcmp(ver, "FIX50")) return FIX50;
   if (!strcmp(ver, "FIX50SP1")) return FIX50SP1;
   if (!strcmp(ver, "FIX50SP2")) return FIX50SP2;
   if (!strcmp(ver, "FIXT11")) return FIXT11;
   return FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT;
}

char const* FIXProtocolVerEnum2BeginString(FIXProtocolVerEnum ver)
{
   switch (ver)
   {
      case FIX42: return "FIX.4.2";
      case FIX44: return "FIX.4.4";
      case FIX50:
      case FIX50SP1:
      case FIX50SP2:
      case FIXT11: return "FIXT.1.1";
      default: return "";
   }
}

FIXFieldTypeEnum str2FIXFIXFieldType(char const* type)
{
   if (!strcmp(type, "Int"))           { return FIXFieldType_Int; }
   if (!strcmp(type, "Length"))        { return FIXFieldType_Length; }
   if (!strcmp(type, "NumInGroup"))    { return FIXFieldType_NumInGroup; }
   if (!strcmp(type, "SeqNum"))        { return FIXFieldType_SeqNum; }
   if (!strcmp(type, "TagNum"))        { return FIXFieldType_TagNum; }
   if (!strcmp(type, "DayOfMonth"))    { return FIXFieldType_DayOfMonth; }
   if (!strcmp(type, "Float"))         { return FIXFieldType_Float; }
   if (!strcmp(type, "Qty"))           { return FIXFieldType_Qty; }
   if (!strcmp(type, "Price"))         { return FIXFieldType_Price; }
   if (!strcmp(type, "PriceOffset"))   { return FIXFieldType_PriceOffset; }
   if (!strcmp(type, "Amt"))           { return FIXFieldType_Amt; }
   if (!strcmp(type, "Percentage"))    { return FIXFieldType_Percentage; }
   if (!strcmp(type, "Char"))          { return FIXFieldType_Char; }
   if (!strcmp(type, "Boolean"))       { return FIXFieldType_Boolean; }
   if (!strcmp(type, "String"))        { return FIXFieldType_String; }
   if (!strcmp(type, "MultipleValueString")) { return FIXFieldType_MultipleValueString; }
   if (!strcmp(type, "Country"))       { return FIXFieldType_Country; }
   if (!strcmp(type, "Currency"))      { return FIXFieldType_Currency; }
   if (!strcmp(type, "Exchange"))      { return FIXFieldType_Exchange; }
   if (!strcmp(type, "MonthYear"))     { return FIXFieldType_MonthYear; }
   if (!strcmp(type, "UTCTimestamp"))  { return FIXFieldType_UTCTimestamp; }
   if (!strcmp(type, "UTCTimeOnly"))   { return FIXFieldType_UTCTimeOnly; }
   if (!strcmp(type, "UTCDateOnly"))   { return FIXFieldType_UTCDateOnly; }
   if (!strcmp(type, "LocalMktDate"))  { return FIXFieldType_LocalMktDate; }
   if (!strcmp(type, "Data"))          { return FIXFieldType_Data; }
   if (!strcmp(type, "TZTimeOnly"))    { return FIXFieldType_TZTimeOnly; }
   if (!strcmp(type, "TZTimestamp"))   { return FIXFieldType_TZTimestamp; }
   if (!strcmp(type, "XMLData"))       { return FIXFieldType_XMLData; }
   if (!strcmp(type, "Language"))      { return FIXFieldType_Language; }
   return FIXFieldType_Unknown;
}
