/* @file   fix_types.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/01/2012 01:53:57 PM
*/

#include "fix_types.h"

#include "string.h"

FIXProtocolVerEnum str2FIXProtocolVerEnum(char const* ver, uint32_t len)
{
   if (!strncmp("FIX.4.2", ver, len)) return FIX42;
   if (!strncmp("FIX.4.4", ver, len)) return FIX44;
   if (!strncmp("FIX.5.0", ver, len)) return FIX50;
   if (!strncmp("FIX.5.0.SP1", ver, len)) return FIX50SP1;
   if (!strncmp("FIX.5.0.SP2", ver, len)) return FIX50SP2;
   if (!strncmp("FIXT.1.1", ver, len)) return FIXT11;
   return FIX_MUST_BE_LAST_DO_NOT_USE_OR_CHANGE_IT;
}

char const* FIXProtocolVerEnum2str(FIXProtocolVerEnum ver)
{
   switch (ver)
   {
      case FIX42: return "FIX.4.2";
      case FIX44: return "FIX.4.4";
      case FIX50: return "FIX.5.0";
      case FIX50SP1: return "FIX.5.0.SP1";
      case FIX50SP2: return "FIX.5.0.SP2";
      case FIXT11: return "FIXT.1.1";
      default: return "";
   }
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

FIXFieldValueTypeEnum str2FIXFieldValueType(char const* type)
{
   if (!strcmp(type, "Int"))           { return FIXFieldValueType_Int; }
   if (!strcmp(type, "Length"))        { return FIXFieldValueType_Length; }
   if (!strcmp(type, "NumInGroup"))    { return FIXFieldValueType_NumInGroup; }
   if (!strcmp(type, "SeqNum"))        { return FIXFieldValueType_SeqNum; }
   if (!strcmp(type, "TagNum"))        { return FIXFieldValueType_TagNum; }
   if (!strcmp(type, "DayOfMonth"))    { return FIXFieldValueType_DayOfMonth; }
   if (!strcmp(type, "Float"))         { return FIXFieldValueType_Float; }
   if (!strcmp(type, "Qty"))           { return FIXFieldValueType_Qty; }
   if (!strcmp(type, "Price"))         { return FIXFieldValueType_Price; }
   if (!strcmp(type, "PriceOffset"))   { return FIXFieldValueType_PriceOffset; }
   if (!strcmp(type, "Amt"))           { return FIXFieldValueType_Amt; }
   if (!strcmp(type, "Percentage"))    { return FIXFieldValueType_Percentage; }
   if (!strcmp(type, "Char"))          { return FIXFieldValueType_Char; }
   if (!strcmp(type, "Boolean"))       { return FIXFieldValueType_Boolean; }
   if (!strcmp(type, "String"))        { return FIXFieldValueType_String; }
   if (!strcmp(type, "MultipleValueString")) { return FIXFieldValueType_MultipleValueString; }
   if (!strcmp(type, "Country"))       { return FIXFieldValueType_Country; }
   if (!strcmp(type, "Currency"))      { return FIXFieldValueType_Currency; }
   if (!strcmp(type, "Exchange"))      { return FIXFieldValueType_Exchange; }
   if (!strcmp(type, "MonthYear"))     { return FIXFieldValueType_MonthYear; }
   if (!strcmp(type, "UTCTimestamp"))  { return FIXFieldValueType_UTCTimestamp; }
   if (!strcmp(type, "UTCTimeOnly"))   { return FIXFieldValueType_UTCTimeOnly; }
   if (!strcmp(type, "UTCDateOnly"))   { return FIXFieldValueType_UTCDateOnly; }
   if (!strcmp(type, "LocalMktDate"))  { return FIXFieldValueType_LocalMktDate; }
   if (!strcmp(type, "Data"))          { return FIXFieldValueType_Data; }
   if (!strcmp(type, "TZTimeOnly"))    { return FIXFieldValueType_TZTimeOnly; }
   if (!strcmp(type, "TZTimestamp"))   { return FIXFieldValueType_TZTimestamp; }
   if (!strcmp(type, "XMLData"))       { return FIXFieldValueType_XMLData; }
   if (!strcmp(type, "Language"))      { return FIXFieldValueType_Language; }
   return FIXFieldValueType_Unknown;
}
