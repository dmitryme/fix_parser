-type attr()      :: {'page_size', pos_integer()} |
                     {'max_page_size', pos_integer()} |
                     {'num_pages', pos_integer()} |
                     {'max_pages', pos_integer()} |
                     {'num_groups', pos_integer()} |
                     {'max_groups', pos_integer()}.  %% parser attributes ...
-type attrs()     :: [attr()].

-type flag()      :: check_crc |
                     check_required |
                     check_value |
                     check_unknown_fields |
                     check_all.
-type flags()     :: [flag()].

-record(parser, {res :: binary()}).

-record(msg,    {type :: binary(),
                 res :: {binary(), binary()}}).

-record(msg_header, {begin_string :: string(),
                     msg_type :: string(),
                     sender_comp_id :: string(),
                     target_comp_id :: string(),
                     msg_seq_num    :: pos_integer()}).

-record(group,  {res :: {binary(), binary(), binary()}}).

-type ref()       :: #msg{} | #group{}.
-type tagNum()    :: pos_integer().
-type reason()    :: atom() |
                     string() |
                     {pos_integer(), string()}.

-define(FIX_SOH,                             1).
-define(FIX_SUCCESS,                         0).
-define(FIX_NO_FIELD,                        1).

-define(FIX_FAILED,                         -1).
-define(FIX_ERROR_FIELD_HAS_WRONG_TYPE,     -2).
-define(FIX_ERROR_FIELD_NOT_FOUND,          -3).
-define(FIX_ERROR_FIELD_TYPE_EXISTS,        -4).
-define(FIX_ERROR_GROUP_WRONG_INDEX,        -5).
-define(FIX_ERROR_XML_ATTR_NOT_FOUND,       -6).
-define(FIX_ERROR_XML_ATTR_WRONG_VALUE,     -7).
-define(FIX_ERROR_PROTOCOL_XML_LOAD_FAILED, -8).
-define(FIX_ERROR_UNKNOWN_FIELD,            -9).
-define(FIX_ERROR_WRONG_PROTOCOL_VER,       -10).
-define(FIX_ERROR_DUPLICATE_FIELD_DESCR,    -11).
-define(FIX_ERROR_UNKNOWN_MSG,              -12).
-define(FIX_ERROR_LIBXML,                   -13).
-define(FIX_ERROR_INVALID_ARGUMENT,         -14).
-define(FIX_ERROR_MALLOC,                   -15).
-define(FIX_ERROR_UNKNOWN_PROTOCOL_DESCR,   -16).
-define(FIX_ERROR_NO_MORE_PAGES,            -17).
-define(FIX_ERROR_NO_MORE_GROUPS,           -18).
-define(FIX_ERROR_TOO_BIG_PAGE,             -19).
-define(FIX_ERROR_NO_MORE_SPACE,            -20).
-define(FIX_ERROR_PARSE_MSG,                -21).
-define(FIX_ERROR_WRONG_FIELD,              -22).
-define(FIX_ERROR_INTEGRITY_CHECK,          -23).
-define(FIX_ERROR_NO_MORE_DATA,             -24).
-define(FIX_ERROR_WRONG_FIELD_VALUE,        -25).
