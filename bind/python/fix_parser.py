from ctypes import *
from types import *
lib = cdll.LoadLibrary('./libfix_parser.so')

FIX_SOH = 1

FIX_SUCCESS                        = 0
FIX_NO_FIELD                       = 1

FIX_FAILED                         = -1
FIX_ERROR_FIELD_HAS_WRONG_TYPE     = -2
FIX_ERROR_FIELD_NOT_FOUND          = -3
FIX_ERROR_FIELD_TYPE_EXISTS        = -4
FIX_ERROR_GROUP_WRONG_INDEX        = -5
FIX_ERROR_XML_ATTR_NOT_FOUND       = -6
FIX_ERROR_XML_ATTR_WRONG_VALUE     = -7
FIX_ERROR_PROTOCOL_XML_LOAD_FAILED = -8
FIX_ERROR_UNKNOWN_FIELD            = -9
FIX_ERROR_WRONG_PROTOCOL_VER       = -10
FIX_ERROR_DUPLICATE_FIELD_DESCR    = -11
FIX_ERROR_UNKNOWN_MSG              = -12
FIX_ERROR_LIBXML                   = -13
FIX_ERROR_INVALID_ARGUMENT         = -14
FIX_ERROR_MALLOC                   = -15
FIX_ERROR_UNKNOWN_PROTOCOL_DESCR   = -16
FIX_ERROR_NO_MORE_PAGES            = -17
FIX_ERROR_NO_MORE_GROUPS           = -18
FIX_ERROR_TOO_BIG_PAGE             = -19
FIX_ERROR_NO_MORE_SPACE            = -20
FIX_ERROR_PARSE_MSG                = -21
FIX_ERROR_WRONG_FIELD              = -22
FIX_ERROR_INTEGRITY_CHECK          = -23
FIX_ERROR_NO_MORE_DATA             = -24
FIX_ERROR_WRONG_FIELD_VALUE        = -25

PARSER_FLAG_CHECK_CRC             = 0x01       # check FIX message CRC during parsing
PARSER_FLAG_CHECK_REQUIRED        = 0x02       # check for required FIX fields
PARSER_FLAG_CHECK_VALUE           = 0x04       # check for valid value.
PARSER_FLAG_CHECK_UNKNOWN_FIELDS  = 0x08       # heck for unknown FIX fields during parsing. If not set all unknown fields ignored
PARSER_FLAG_CHECK_ALL             = PARSER_FLAG_CHECK_CRC | PARSER_FLAG_CHECK_REQUIRED | PARSER_FLAG_CHECK_VALUE | PARSER_FLAG_CHECK_UNKNOWN_FIELDS # make all possible checks

def raise_error(err):
   c_err_code = lib.fix_error_get_code(err)
   c_err_txt = lib.fix_error_get_text(err)
   err_txt = string_at(c_err_txt)
   lib.fix_error_free(err)
   raise Exception(c_err_code, err_txt)

class FixParser(object):
   def __init__(self, protFile, flags):
      error = c_long(0)
      self.parser = lib.fix_parser_create(protFile, 0, flags, pointer(error))
      if not self.parser:
         raise_error(error)

   def __exit__(self, type, value, traceback):
      lib.fix_parser_free(self.parser)

   def getProtocolVer(self):
      ver = lib.fix_parser_get_protocol_ver(self.parser)
      return string_at(ver)

   def createMsg(self, type):
      error = c_long(0)
      msg = lib.fix_msg_create(self.parser, type, pointer(error))
      if not msg:
         raise_error(error)
      return FixMessage(msg)

   def parse(self, data, delimiter):
      error = c_long(0)
      stop = c_char()
      msg = lib.fix_parser_str_to_msg(self.parser, c_char_p(data), c_uint(len(data)), c_byte(delimiter),
            pointer(stop), pointer(error))
      if not msg:
         raise_error(error)
      return FixMessage(msg)

class FixGroup(object):
   msg = 0
   group = 0
   def __init__(self, msg, group):
      self.msg = msg
      self.group = group

   def setFieldAsString(self, fieldTag, value):
      error = c_long(0)
      ret = lib.fix_msg_set_string(self.msg, self.group, fieldTag, value, pointer(error))
      if ret:
         raise_error(error)

   def setFieldAsInt32(self, fieldTag, value):
      error = c_long(0)
      ret = lib.fix_msg_set_int32(self.msg, self.group, fieldTag, value, pointer(error))
      if ret:
         raise_error(error)

   def setFieldAsInt64(self, fieldTag, value):
      error = c_long(0)
      ret = lib.fix_msg_set_int64(self.msg, self.group, fieldTag, value, pointer(error))
      if ret:
         raise_error(error)

   def setFieldAsChar(self, fieldTag, value):
      error = c_long(0)
      ret = lib.fix_msg_set_char(self.msg, self.group, fieldTag, c_char(value[0]), pointer(error))
      if ret:
         raise_error(error)

   def setFieldAsDouble(self, fieldTag, value):
      error = c_long(0)
      ret = lib.fix_msg_set_double(self.msg, self.group, fieldTag, c_double(value), pointer(error))
      if ret:
         raise_error(error)

   def getFieldAsInt32(self, fieldTag, defVal = 0):
      error = c_long(0)
      val = c_int32(0)
      ret = lib.fix_msg_get_int32(self.msg, self.group, fieldTag, pointer(val), defVal, pointer(error))
      if ret:
         raise_error(error)
      return val.value

   def getFieldAsInt64(self, fieldTag, defVal = 0):
      error = c_long(0)
      ptr = pointer(error)
      val = c_int64(0)
      ret = lib.fix_msg_get_int64(self.msg, self.group, fieldTag, pointer(val), defVal, pointer(error))
      if ret:
         raise_error(error)
      return val.value

   def getFieldAsDouble(self, fieldTag, defVal = 0.0):
      error = c_long(0)
      val = c_double(0.0)
      ret = lib.fix_msg_get_double(self.msg, self.group, fieldTag, pointer(val), pointer(error))
      if ret:
         raise_error(error)
      return val.value

   def getFieldAsChar(self, fieldTag, defVal = 0):
      error = c_long(0)
      val = c_char()
      ret = lib.fix_msg_get_char(self.msg, self.group, fieldTag, pointer(val), pointer(error))
      if ret:
         raise_error(error)
      return val.value

   def getFieldAsString(self, fieldTag, defVal = ''):
      error = c_long(0)
      val = c_int(0)
      length = c_int(0)
      ret = lib.fix_msg_get_string(
            self.msg, self.group, fieldTag, pointer(val), pointer(length), defVal,
            len(defVal), pointer(error))
      if ret:
         raise_error(error)
      return string_at(val.value, length.value)

   def addGroup(self, fieldTag):
      error = c_long(0)
      group = lib.fix_msg_add_group(self.msg, self.group, fieldTag, pointer(error))
      if not group:
         raise_error(error)
      return FixGroup(self.msg, group)

   def delGroup(self, fieldTag, groupIdx):
      error = c_long(0)
      ret = lib.fix_msg_del_group(self.msg, self.group, fieldTag, groupIdx, pointer(error))
      if ret:
         raise_error(error)

   def delField(self, fieldTag):
      error = c_long(0)
      ret = lib.fix_msg_del_field(self.msg, self.group, fieldTag, pointer(error))
      if ret:
         raise_error(error)

class FixMessage(FixGroup):
   def __init__(self, msg):
      self.msg = msg
      FixGroup(msg, 0)

   def __exit__(self, type, value, traceback):
      lib.fix_msg_free(self.obj)

   def getType(self):
      return string_at(lib.fix_msg_get_type(self.msg))

   def getName(self):
      return string_at(lib.fix_msg_get_name(self.msg))

   def toString(self, delimiter):
      error = c_long(0)
      buf = create_string_buffer(256)
      reqLen = c_uint(0)
      ret = lib.fix_msg_to_str(self.msg, c_byte(delimiter), buf, len(buf), pointer(reqLen), pointer(error))
      if ret:
         if not error and reqLen > len(buf):
            buf = create_string_buffer(reqLen.value)
            ret = lib.fix_msg_to_str(self.msg, c_byte(delimiter), buf, len(buf), pointer(reqLen), pointer(error))
            if ret:
               raise_error(error)
         else:
            raise_error(error)
      return buf.value
