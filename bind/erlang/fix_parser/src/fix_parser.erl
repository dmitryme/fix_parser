-module(fix_parser).

-include("fix_parser.hrl").

-export([create/3, get_version/1, create_msg/2, add_group/2, get_group/3, del_group/3,
      set_int32_field/3, set_int64_field/3, set_double_field/3, set_string_field/3, set_char_field/3, set_data_field/3,
      get_int32_field/2, get_int32_field/3, get_int64_field/2, get_int64_field/3, get_double_field/2, get_double_field/3,
      get_string_field/2, get_string_field/3, get_char_field/2, get_char_field/3, get_data_field/2, get_data_field/3,
      get_header/2, msg_to_binary/2, binary_to_msg/3]).

-on_load(load_lib/0).

load_lib() ->
   erlang:load_nif(code:priv_dir(fix_parser) ++ "/fix_parser", 0).

-spec create(string(), attrs(), flags()) -> {ok, #parser{}} | {error, reason()}.
create(_Path, _Attrs, _Flags) ->
   {error, library_not_loaded}.

-spec get_version(#parser{}) -> string().
get_version(_ParserRef) ->
   {error, library_not_loaded}.

-spec create_msg(#parser{}, string()) -> {ok, #msg{}} | {error, reason()}.
create_msg(_ParserRef, _MsgType) ->
   {error, library_not_loaded}.

-spec add_group(ref(), tagNum()) -> {ok, #group{}} | {error, reason()}.
add_group(_Ref, _TagNum) ->
   {error, library_not_loaded}.

-spec get_group(ref(), tagNum(), pos_integer()) -> {ok, #group{}} | {error, reason()}.
get_group(_Ref, _TagNum, _Idx) ->
   {error, library_not_loaded}.

-spec del_group(ref(), tagNum(), pos_integer()) -> ok | {error, reason()}.
del_group(_Ref, _TagNum, _Idx) ->
   {error, library_not_loaded}.

-spec set_int32_field(ref(), tagNum(), integer()) -> ok | {error, reason()}.
set_int32_field(_MsgRef, _TagNum, _Value) ->
   {error, library_not_loaded}.

-spec set_int64_field(ref(), tagNum(), integer()) -> ok | {error, reason()}.
set_int64_field(_MsgRef, _FieldNum, _Value) ->
   {error, library_not_loaded}.

-spec set_double_field(ref(), tagNum(), float()) -> ok | {error, reason()}.
set_double_field(_MsgRef, _TagNum, _Value) ->
   {error, library_not_loaded}.

-spec set_string_field(ref(), tagNum(), string()) -> ok | {error, reason()}.
set_string_field(_MsgRef, _TagNum, _Value) ->
   {error, library_not_loaded}.

-spec set_char_field(ref(), tagNum(), char()) -> ok | {error, reason()}.
set_char_field(_MsgRef, _TagNum, _Value) ->
   {error, library_not_loaded}.

-spec set_data_field(ref(), tagNum(), binary()) -> ok | {error, reason()}.
set_data_field(_MsgRef, _TagNum, _Value) ->
   {error, library_not_loaded}.

-spec get_int32_field(ref(), tagNum()) -> {ok, integer()} | {error, reason()}.
get_int32_field(_MsgRef, _TagNum) ->
   {error, library_not_loaded}.

-spec get_int32_field(ref(), tagNum(), integer()) -> {ok, integer()} | {error, reason()}.
get_int32_field(MsgRef, TagNum, DefValue) ->
   case get_int32_field(MsgRef, TagNum) of
      V = {ok, _Value} ->
         V;
      {fix_error, ?FIX_ERROR_FIELD_NOT_FOUND, _} ->
         {ok, DefValue};
      Err ->
         Err
   end.

-spec get_int64_field(ref(), tagNum()) -> {ok, integer()} | {error, reason()}.
get_int64_field(_MsgRef, _TagNum) ->
   {error, library_not_loaded}.

-spec get_int64_field(ref(), tagNum(), integer()) -> {ok, integer()} | {error, reason()}.
get_int64_field(MsgRef, TagNum, DefValue) ->
   case get_int64_field(MsgRef, TagNum) of
      V = {ok, _Value} ->
         V;
      {fix_error, ?FIX_ERROR_FIELD_NOT_FOUND, _} ->
         {ok, DefValue};
      Err ->
         Err
   end.

-spec get_double_field(ref(), tagNum()) -> {ok, float()} | {error, reason()}.
get_double_field(_MsgRef, _TagNum) ->
   {error, library_not_loaded}.

-spec get_double_field(ref(), tagNum(), float()) -> {ok, float()} | {error, reason()}.
get_double_field(MsgRef, TagNum, DefValue) ->
   case get_double_field(MsgRef, TagNum) of
      V = {ok, _Value} ->
         V;
      {fix_error, ?FIX_ERROR_FIELD_NOT_FOUND, _} ->
         {ok, DefValue};
      Err ->
         Err
   end.

-spec get_string_field(ref(), tagNum()) -> {ok, string()} | {error, reason()}.
get_string_field(_MsgRef, _TagNum) ->
   {error, library_not_loaded}.

-spec get_string_field(ref(), tagNum(), string()) -> {ok, string()} | {error, reason()}.
get_string_field(MsgRef, TagNum, DefValue) ->
   case get_string_field(MsgRef, TagNum) of
      V = {ok, _Value} ->
         V;
      {fix_error, ?FIX_ERROR_FIELD_NOT_FOUND, _} ->
         {ok, DefValue};
      Err ->
         Err
   end.

-spec get_char_field(ref(), tagNum()) -> {ok, char()} | {error, reason()}.
get_char_field(_MsgRef, _TagNum) ->
   {error, library_not_loaded}.

-spec get_char_field(ref(), tagNum(), char()) -> {ok, char()} | {error, reason()}.
get_char_field(MsgRef, TagNum, DefValue) ->
   case get_char_field(MsgRef, TagNum) of
      V = {ok, _Value} ->
         V;
      {fix_error, ?FIX_ERROR_FIELD_NOT_FOUND, _} ->
         {ok, DefValue};
      Err ->
         Err
   end.

-spec get_data_field(ref(), tagNum()) -> {ok, binary()} | {error, reason()}.
get_data_field(_MsgRef, _TagNum) ->
   {error, library_not_loaded}.

-spec get_data_field(ref(), tagNum(), binary()) -> {ok, binary()} | {error, reason()}.
get_data_field(MsgRef, TagNum, DefValue) ->
   case get_data_field(MsgRef, TagNum) of
      V = {ok, _Value} ->
         V;
      {fix_error, ?FIX_ERROR_FIELD_NOT_FOUND, _} ->
         {ok, DefValue};
      Err ->
         Err
   end.

-spec get_header(binary(), char()) -> {string(), string()}.
get_header(_BinData, _Delimiter) ->
   {error, library_not_loaded}.

-spec msg_to_binary(#msg{}, char()) -> {ok, binary()} | {error, reason()}.
msg_to_binary(_MsgRef, _Delimiter) ->
   {error, library_not_loaded}.

-spec binary_to_msg(#parser{}, char(), binary()) -> {ok, #msg{}, binary()} | {error, reason()}.
binary_to_msg(_ParserRef, _Delimiter, _BinData) ->
   {error, library_not_loaded}.

-ifdef(TEST).

-include_lib("eunit/include/eunit.hrl").
-include("fix_fields.hrl").

test1_test() ->
   {ok, P} = fix_parser:create("../../../../fix_descr/fix.4.4.xml", [], [check_all]),
   {ok, M} = fix_parser:create_msg(P, "8"),
   ?assertEqual("FIX.4.4", fix_parser:get_version(P)),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_SenderCompID, "QWERTY_12345678")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_TargetCompID, "ABCQWE_XYZ")),
   ?assertEqual(ok, fix_parser:set_int32_field(M,  ?FIXFieldTag_MsgSeqNum, 34)),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_TargetSubID, "srv-ivanov_ii1")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_SendingTime, "20120716-06:00:16.230")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_OrderID, "1")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_ClOrdID, "CL_ORD_ID_1234567")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_ExecID, "FE_1_9494_1")),
   ?assertEqual(ok, fix_parser:set_char_field(M,   ?FIXFieldTag_ExecType, $0)),
   ?assertEqual(ok, fix_parser:set_char_field(M,   ?FIXFieldTag_OrdStatus, $1)),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_Account, "ZUM")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_Symbol, "RTS-12.12")),
   ?assertEqual(ok, fix_parser:set_char_field(M,   ?FIXFieldTag_Side, $1)),
   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_OrderQty, 25.0)),
   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_Price, 135155.0)),
   ?assertEqual(ok, fix_parser:set_char_field(M,   ?FIXFieldTag_TimeInForce, $0)),
   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_LastQty, 0)),
   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_LastPx, 0)),
   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_LeavesQty, 25.0)),
   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_CumQty, 0)),
   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_AvgPx, 0)),
   ?assertEqual(ok, fix_parser:set_char_field(M,   ?FIXFieldTag_HandlInst, $1)),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_Text, "COMMENT12")),
   {ok, Fix} = fix_parser:msg_to_binary(M, ?FIX_SOH),
   Res = {ok, M1, _} = fix_parser:binary_to_msg(P, ?FIX_SOH, Fix),
   ?assertMatch({ok, _, <<>>}, Res),
   ?assertEqual({ok, "QWERTY_12345678"}, fix_parser:get_string_field(M1, ?FIXFieldTag_SenderCompID)),
   ?assertEqual({ok, "ABCQWE_XYZ"}, fix_parser:get_string_field(M1, ?FIXFieldTag_TargetCompID)),
   ?assertEqual({ok, 34}, fix_parser:get_int32_field(M1, ?FIXFieldTag_MsgSeqNum)),
   ?assertEqual({ok, "srv-ivanov_ii1"}, fix_parser:get_string_field(M1, ?FIXFieldTag_TargetSubID)),
   ?assertEqual({ok, "20120716-06:00:16.230"}, fix_parser:get_string_field(M1, ?FIXFieldTag_SendingTime)),
   ?assertEqual({ok, "1"}, fix_parser:get_string_field(M1, ?FIXFieldTag_OrderID)),
   ?assertEqual({ok, "CL_ORD_ID_1234567"}, fix_parser:get_string_field(M1, ?FIXFieldTag_ClOrdID)),
   ?assertEqual({ok, "FE_1_9494_1"}, fix_parser:get_string_field(M1, ?FIXFieldTag_ExecID)),
   ?assertEqual({ok, $0}, fix_parser:get_char_field(M1, ?FIXFieldTag_ExecType)),
   ?assertEqual({ok, $1}, fix_parser:get_char_field(M1, ?FIXFieldTag_OrdStatus)),
   ?assertEqual({ok, "ZUM"}, fix_parser:get_string_field(M1, ?FIXFieldTag_Account)),
   ?assertEqual({ok, "RTS-12.12"}, fix_parser:get_string_field(M1, ?FIXFieldTag_Symbol)),
   ?assertEqual({ok, $1}, fix_parser:get_char_field(M1, ?FIXFieldTag_Side)),
   ?assertEqual({ok, 25.0}, fix_parser:get_double_field(M1, ?FIXFieldTag_OrderQty)),
   ?assertEqual({ok, 135155.00}, fix_parser:get_double_field(M1, ?FIXFieldTag_Price)),
   ?assertEqual({ok, $0}, fix_parser:get_char_field(M1, ?FIXFieldTag_TimeInForce)),
   ?assertEqual({ok, 0.0}, fix_parser:get_double_field(M1, ?FIXFieldTag_LastQty)),
   ?assertEqual({ok, 0.0}, fix_parser:get_double_field(M1, ?FIXFieldTag_LastPx)),
   ?assertEqual({ok, 25.0}, fix_parser:get_double_field(M1, ?FIXFieldTag_LeavesQty)),
   ?assertEqual({ok, 0.0}, fix_parser:get_double_field(M1, ?FIXFieldTag_CumQty)),
   ?assertEqual({ok, 0.0}, fix_parser:get_double_field(M1, ?FIXFieldTag_AvgPx)),
   ?assertEqual({ok, $1}, fix_parser:get_char_field(M1, ?FIXFieldTag_HandlInst)),
   ?assertEqual({ok, "COMMENT12"}, fix_parser:get_string_field(M1, ?FIXFieldTag_Text)),
   ?assertEqual({ok, "DEFAULT"}, fix_parser:get_string_field(M1, ?FIXFieldTag_SymbolSfx, "DEFAULT")),
   ?assertEqual({ok, 12345}, fix_parser:get_int32_field(M1, ?FIXFieldTag_NoOrders, 12345)),
   ?assertEqual({ok, 12345}, fix_parser:get_int64_field(M1, ?FIXFieldTag_NoOrders, 12345)),
   ?assertEqual({ok, 12345.123}, fix_parser:get_double_field(M1, ?FIXFieldTag_NoOrders, 12345.123)),
   ?assertEqual({ok, $A}, fix_parser:get_char_field(M1, ?FIXFieldTag_NoOrders, $A)).

test2_test() ->
   {ok, P} = fix_parser:create("../../../../fix_descr/fix.5.0.sp2.xml", [], []),
   {ok, M} = fix_parser:create_msg(P, "AE"),
   ?assertEqual("FIX.5.0.SP2", fix_parser:get_version(P)),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_ApplVerID, "9")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_TradeReportID, "121111_1_3999")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_TradeID, "121111_1_3999")),
   ?assertEqual(ok, fix_parser:set_int32_field(M, ?FIXFieldTag_TradeReportTransType, 0)),
   ?assertEqual(ok, fix_parser:set_int32_field(M, ?FIXFieldTag_TradeReportType, 0)),
   ?assertEqual(ok, fix_parser:set_int32_field(M, ?FIXFieldTag_TrdType, 0)),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_OrigTradeID, "121119_1_3999")),
   ?assertEqual(ok, fix_parser:set_char_field(M, ?FIXFieldTag_ExecType, $F)),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_ExecID, "0")),
   ?assertEqual(ok, fix_parser:set_int32_field(M, ?FIXFieldTag_PriceType, 2)),
   {ok, RootPartyID1} = fix_parser:add_group(M, ?FIXFieldTag_NoRootPartyIDs),
   ?assertEqual(ok, fix_parser:set_string_field(RootPartyID1, ?FIXFieldTag_RootPartyID, "XYZ")),
   ?assertEqual(ok, fix_parser:set_char_field(RootPartyID1, ?FIXFieldTag_RootPartyIDSource, $D)),
   ?assertEqual(ok, fix_parser:set_int32_field(RootPartyID1, ?FIXFieldTag_RootPartyRole, 16)),
   {ok, RootPartyID2} = fix_parser:add_group(M, ?FIXFieldTag_NoRootPartyIDs),
   ?assertEqual(ok, fix_parser:set_string_field(RootPartyID2, ?FIXFieldTag_RootPartyID, "XYZA")),
   ?assertEqual(ok, fix_parser:set_char_field(RootPartyID2, ?FIXFieldTag_RootPartyIDSource, $E)),
   ?assertEqual(ok, fix_parser:set_int32_field(RootPartyID2, ?FIXFieldTag_RootPartyRole, 17)),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_MarketID, "OTC")),
   {ok, SecAltID} = fix_parser:add_group(M, ?FIXFieldTag_NoSecurityAltID),
   ?assertEqual(ok, fix_parser:set_string_field(SecAltID, ?FIXFieldTag_SecurityAltID, "SYMBOL_ABC")),
   ?assertEqual(ok, fix_parser:set_string_field(SecAltID, ?FIXFieldTag_SecurityAltID, "M")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_CFICode, "MRCSXX")),
   ?assertEqual(ok, fix_parser:set_int32_field(M, ?FIXFieldTag_QtyType, 0)),
   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_LastQty, 110000)),
   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_LastQty, 31.12)),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_Currency, "USD")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_TradeDate, "20121119")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_TransactTime, "20121119-08:33:57")),
   ?assertEqual(ok, fix_parser:set_string_field(M, ?FIXFieldTag_SettlType, "2")),
   {ok, SideGrp} = fix_parser:add_group(M, ?FIXFieldTag_NoSides),
   ?assertEqual(ok, fix_parser:set_char_field(SideGrp, ?FIXFieldTag_Side, $1)),
   ?assertEqual(ok, fix_parser:set_string_field(SideGrp, ?FIXFieldTag_OrderID, "ORD_1234567")),
   ?assertEqual(ok, fix_parser:set_string_field(SideGrp, ?FIXFieldTag_ClOrdID, "CLORDID_1234567")),
   ?assertEqual(ok, fix_parser:set_char_field(SideGrp, ?FIXFieldTag_OrdType, $2)),
   ?assertEqual(ok, fix_parser:set_double_field(SideGrp, ?FIXFieldTag_Price, 31.12)),
   ?assertEqual(ok, fix_parser:set_double_field(SideGrp, ?FIXFieldTag_OrderQty, 5000000)),
   ?assertEqual(ok, fix_parser:set_double_field(SideGrp, ?FIXFieldTag_LeavesQty, 444000)),
   ?assertEqual(ok, fix_parser:set_double_field(SideGrp, ?FIXFieldTag_CumQty, 1111000)),

   {ok, PartyGrp1} = fix_parser:add_group(SideGrp, ?FIXFieldTag_NoPartyIDs),
   ?assertEqual(ok, fix_parser:set_string_field(PartyGrp1, ?FIXFieldTag_PartyID, "FX_FF_FLXX")),
   ?assertEqual(ok, fix_parser:set_char_field(PartyGrp1, ?FIXFieldTag_PartyIDSource, $D)),
   ?assertEqual(ok, fix_parser:set_int32_field(PartyGrp1, ?FIXFieldTag_PartyRole, 38)),

   {ok, PartyGrp2} = fix_parser:add_group(SideGrp, ?FIXFieldTag_NoPartyIDs),
   ?assertEqual(ok, fix_parser:set_string_field(PartyGrp2, ?FIXFieldTag_PartyID, "FX_FF_FLYY")),
   ?assertEqual(ok, fix_parser:set_char_field(PartyGrp2, ?FIXFieldTag_PartyIDSource, $D)),
   ?assertEqual(ok, fix_parser:set_int32_field(PartyGrp2, ?FIXFieldTag_PartyRole, 41)),

   ?assertEqual(ok, fix_parser:set_double_field(M, ?FIXFieldTag_GrossTradeAmt, 357333.12)).

test3_test() ->
   Logon = <<"8=FIX.4.4\0019=139\00135=A\00149=dmelnikov1_test_robot1\00156=crossing_engine\00134=1\00152=20130130-14:50:33.448\00198=0\001108=30\001141=Y\001553=dmelnikov\001554=xlltlib(1.0):dmelnikov\00110=196\001">>,
   {ok, Header} = fix_parser:get_header(Logon, 1),
   ?assertEqual(Header#msg_header.begin_string, "FIX.4.4"),
   ?assertEqual(Header#msg_header.msg_type, "A"),
   ?assertEqual(Header#msg_header.sender_comp_id, "dmelnikov1_test_robot1"),
   ?assertEqual(Header#msg_header.target_comp_id, "crossing_engine"),
   ?assertEqual(Header#msg_header.msg_seq_num, 1).

-endif.
