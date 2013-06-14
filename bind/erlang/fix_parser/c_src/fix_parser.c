/**
 * @file   fix_parser.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 11/07/2012 17:54:30 AM
 */

#include <erl_nif.h>
#include <linux/limits.h> // for PATH_MAX const
#include <fix_parser.h>
#include <fix_error.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <pthread.h>
#include <assert.h>

#define DEF_BINARY_SIZE 1024

static ERL_NIF_TERM ok_atom;
static ERL_NIF_TERM error_atom;
static ERL_NIF_TERM parser_atom;
static ERL_NIF_TERM msg_atom;
static ERL_NIF_TERM group_atom;
static ERL_NIF_TERM msg_header;
static ErlNifResourceType* parser_res;
static ErlNifResourceType* message_res;
static ErlNifResourceType* group_res;

typedef struct ParserRes
{
   FIXParser* ptr;
   pthread_rwlock_t lock;
} ParserRes;

typedef struct MsgRes
{
   FIXMsg* msg;
   pthread_rwlock_t* lock;
} MsgRes;

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM make_error(ErlNifEnv* env, int32_t errCode, char const* errorMsg, ...)
{
   va_list ap;
   va_start(ap, errorMsg);
   char text[1024];
   vsnprintf(text, sizeof(text), errorMsg, ap);
   return enif_make_tuple3(env, error_atom, enif_make_int(env, errCode), enif_make_string(env, text, ERL_NIF_LATIN1));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM make_parser_error(ErlNifEnv* env, int32_t errCode, char const* errText)
{
   return enif_make_tuple3(env, error_atom, enif_make_int(env, errCode), enif_make_string(env, errText, ERL_NIF_LATIN1));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static void free_fix_parser(ErlNifEnv* env, void* obj)
{
   fix_parser_free(*(FIXParser**)obj);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static void free_fix_msg(ErlNifEnv* env, void* obj)
{
   MsgRes* msgRes = (MsgRes*)obj;
   pthread_rwlock_wrlock(msgRes->lock);
   fix_msg_free(msgRes->msg);
   pthread_rwlock_unlock(msgRes->lock);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static int32_t load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
   parser_res = enif_open_resource_type( env, NULL, "erlang_fix_parser_res",
      free_fix_parser, ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER, NULL);
   message_res = enif_open_resource_type( env, NULL, "erlang_fix_message_res",
      free_fix_msg, ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER, NULL);
   group_res = enif_open_resource_type( env, NULL, "erlang_fix_group_res",
      NULL, ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER, NULL);
   ok_atom = enif_make_atom(env, "ok");
   error_atom = enif_make_atom(env, "fix_error");
   parser_atom = enif_make_atom(env, "parser");
   msg_atom = enif_make_atom(env, "msg");
   group_atom = enif_make_atom(env, "group");
   msg_header = enif_make_atom(env, "msg_header");
   return 0;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_parser(ErlNifEnv* env, ERL_NIF_TERM ref, ERL_NIF_TERM* pres, ParserRes** parser)
{
   ERL_NIF_TERM const* tuple = NULL;
   int32_t arity;
   if (!enif_get_tuple(env, ref, &arity, &tuple) || arity != 2)
   {
      return make_error(env, FIX_FAILED, "Wrong parser reference.");
   }
   if (enif_compare(tuple[0], parser_atom))
   {
      return make_error(env, FIX_FAILED, "Wrong parser reference.");
   }
   *pres = tuple[1];
   void* res = NULL;
   if (!enif_get_resource(env, *pres, parser_res, &res))
   {
      return make_error(env, FIX_FAILED, "Wrong parser resource.");
   }
   *parser = (ParserRes*)res;
   return ok_atom;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_parser_msg(
      ErlNifEnv* env, ERL_NIF_TERM ref,
      ERL_NIF_TERM* pres, ERL_NIF_TERM* mres,
      ParserRes** parser, FIXMsg** msg)
{
   int32_t arity;
   ERL_NIF_TERM const* tuple = NULL;
   if (!enif_get_tuple(env, ref, &arity, &tuple) || arity != 3)
   {
      return make_error(env, FIX_FAILED, "Wrong msg reference.");
   }
   if (enif_compare(tuple[0], msg_atom))
   {
      return make_error(env, FIX_FAILED, "Wrong msg reference.");
   }
   ERL_NIF_TERM const* data = NULL;
   if (!enif_get_tuple(env, tuple[2], &arity, &data) || arity != 2)
   {
      return make_error(env, FIX_FAILED, "Wrong msg reference.");
   }
   *pres = data[0];
   *mres = data[1];
   void* res = NULL;
   if (!enif_get_resource(env, *pres, parser_res, &res))
   {
      return make_error(env, FIX_FAILED, "Wrong parser resource.");
   }
   *parser = (ParserRes*)res;

   if (!enif_get_resource(env, *mres, message_res, &res))
   {
      return make_error(env, FIX_FAILED, "Wrong message resource.");
   }
   *msg = ((MsgRes*)res)->msg;
   return ok_atom;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_parser_msg_group(
      ErlNifEnv* env, ERL_NIF_TERM ref,
      ERL_NIF_TERM* pres, ERL_NIF_TERM* mres, ERL_NIF_TERM* gres,
      ParserRes** parser, FIXMsg** msg, FIXGroup** group)
{
   int32_t arity;
   ERL_NIF_TERM const* tuple = NULL;
   if (!enif_get_tuple(env, ref, &arity, &tuple) || (arity != 2 && arity != 3))
   {
      return make_error(env, FIX_FAILED, "Wrong msg reference1.");
   }
   if (enif_compare(tuple[0], msg_atom) && enif_compare(tuple[0], group_atom))
   {
      return make_error(env, FIX_FAILED, "Wrong msg reference2.");
   }
   ERL_NIF_TERM const* data = NULL;
   if (arity == 3) // this is a message
   {
      if (!enif_get_tuple(env, tuple[2], &arity, &data) || arity != 2)
      {
         return make_error(env, FIX_FAILED, "Wrong msg reference3.");
      }
   }
   else // this is a group
   {
      if (!enif_get_tuple(env, tuple[1], &arity, &data) || arity != 3)
      {
         return make_error(env, FIX_FAILED, "Wrong msg reference4.");
      }
   }
   *pres = data[0];
   *mres = data[1];
   if (arity == 3) // group exists
   {
      *gres = data[2];
   }
   void* res = NULL;
   if (!enif_get_resource(env, *pres, parser_res, &res))
   {
      return make_error(env, FIX_FAILED, "Wrong parser resource.");
   }
   *parser = (ParserRes*)res;

   if (!enif_get_resource(env, *mres, message_res, &res))
   {
      return make_error(env, FIX_FAILED, "Wrong message resource.");
   }
   *msg = ((MsgRes*)res)->msg;
   if (arity == 3) // group exists
   {
      if (!enif_get_resource(env, *gres, group_res, &res))
      {
         return make_error(env, FIX_FAILED, "Wrong group resource.");
      }
      *group = *(FIXGroup**)res;
   }
   else
   {
      *group = NULL;
   }
   return ok_atom;
}


/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM create(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   char path[PATH_MAX] = {};
   int32_t res = enif_get_string(env, argv[0], path, sizeof(path), ERL_NIF_LATIN1);
   if (res <= 0)
   {
      return make_error(env, FIX_FAILED, "First paremeter is invalid. Should be string.");
   }
   FIXParserAttrs attrs = {};
   ERL_NIF_TERM head;
   ERL_NIF_TERM tail = argv[1];
   while(enif_get_list_cell(env, tail, &head, &tail))
   {
      int32_t arity;
      ERL_NIF_TERM const* tuple = NULL;
      if (!enif_get_tuple(env, head, &arity, &tuple) || arity != 2)
      {
         return make_error(env, FIX_FAILED, "Wrong param arity. Should be tuple with arity 2.");
      }
      char pname[64] = {};
      if (!enif_get_atom(env, tuple[0], pname, sizeof(pname), ERL_NIF_LATIN1))
      {
         return make_error(env, FIX_FAILED, "Wrong param name type. Should be atom.");
      }
      int32_t val = 0;
      if (!enif_get_int(env, tuple[1], &val))
      {
         return make_error(env, FIX_FAILED, "Wrong param '%s' value. Should be integer.", pname);
      }
      if (!strcmp("page_size", pname)) { attrs.pageSize = val; }
      else if (!strcmp("max_page_size", pname)) { attrs.maxPageSize = val; }
      else if (!strcmp("num_pages", pname)) { attrs.numPages = val; }
      else if (!strcmp("num_pages", pname)) { attrs.numPages = val; }
      else if (!strcmp("max_pages", pname)) { attrs.maxPages = val; }
      else if (!strcmp("num_groups", pname)) { attrs.numGroups = val; }
      else if (!strcmp("max_groups", pname)) { attrs.maxGroups = val; }
      else
      {
         return make_error(env, FIX_FAILED, "Unsupported parameter name '%s'.", pname);
      }
   }
   tail = argv[2];
   int32_t flags = 0;
   while(enif_get_list_cell(env, tail, &head, &tail))
   {
      char flag[64] = {};
      enif_get_atom(env, head, flag, sizeof(flag), ERL_NIF_LATIN1);
      if (!strcmp(flag, "check_crc")) { flags |= PARSER_FLAG_CHECK_CRC; }
      else if (!strcmp(flag, "check_required")) { flags |= PARSER_FLAG_CHECK_REQUIRED; }
      else if (!strcmp(flag, "check_value")) { flags |= PARSER_FLAG_CHECK_VALUE; }
      else if (!strcmp(flag, "check_unknown_fields")) { flags |= PARSER_FLAG_CHECK_UNKNOWN_FIELDS; }
      else if (!strcmp(flag, "check_all")) { flags |= PARSER_FLAG_CHECK_ALL; }
      else
      {
         return make_error(env, FIX_FAILED, "Unsupported flag '%s'.", flag);
      }
   }
   FIXError* error = NULL;
   FIXParser* parser = fix_parser_create(path, &attrs, flags, &error);
   if (!parser)
   {
      ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return ret;
   }
   pthread_rwlockattr_t attr;
   pthread_rwlock_t lock;
   int err = pthread_rwlockattr_init(&attr);
   if (err)
   {
      fix_parser_free(parser);
      return make_error(env, FIX_FAILED, "Unable to init lock attr. Error = %s", strerror(err));
   }
   err = pthread_rwlock_init(&lock, &attr);
   if (err)
   {
      fix_parser_free(parser);
      return make_error(env, FIX_FAILED, "Unable to init mutex. Error = %s", strerror(err));
   }
   pthread_rwlockattr_destroy(&attr);
   ParserRes* pres = (ParserRes*)enif_alloc_resource(parser_res, sizeof(ParserRes));
   pres->ptr = parser;
   pres->lock = lock;
   ERL_NIF_TERM pres_term = enif_make_resource(env, pres);
   enif_release_resource(pres);
   return enif_make_tuple2(env, ok_atom, enif_make_tuple2(env, parser_atom, pres_term));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_version(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM pres;
   ParserRes* parser = NULL;
   ERL_NIF_TERM res = get_parser(env, argv[0], &pres, &parser);
   if (res != ok_atom)
   {
      return res;
   }
   char const* ver = fix_parser_get_protocol_ver(parser->ptr);
   return enif_make_string(env, ver, ERL_NIF_LATIN1);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM create_msg(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM pres;
   ParserRes* parser = NULL;
   ERL_NIF_TERM res = get_parser(env, argv[0], &pres, &parser);
   if (res != ok_atom)
   {
      return res;
   }
   char msgType[12] = {};
   if (enif_get_string(env, argv[1], msgType, sizeof(msgType), ERL_NIF_LATIN1) <= 0)
   {
      return make_error(env, FIX_FAILED, "Wrong msgType.");
   }
   FIXError* error = NULL;
   pthread_rwlock_wrlock(&parser->lock);
   FIXMsg* msg = fix_msg_create(parser->ptr, msgType, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (!msg)
   {
      ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return ret;
   }
   MsgRes* msg_res = (MsgRes*)enif_alloc_resource(message_res, sizeof(MsgRes));
   msg_res->msg = msg;
   msg_res->lock = &parser->lock;
   ERL_NIF_TERM msg_term = enif_make_resource(env, msg_res);
   enif_release_resource(msg_res);
   return enif_make_tuple2(
         env,
         ok_atom,
         enif_make_tuple3(env,
            msg_atom,
            enif_make_string(env, msgType, ERL_NIF_LATIN1),
            enif_make_tuple2(env, pres, msg_term)));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM add_group(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM pres;
   ERL_NIF_TERM mres;
   ERL_NIF_TERM gres;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &pres, &mres, &gres, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   FIXError* error = NULL;
   pthread_rwlock_wrlock(&parser->lock);
   FIXGroup* new_group = fix_msg_add_group(msg, group, tagNum, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (!new_group)
   {
      ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return ret;
   }
   FIXGroup** grp = (FIXGroup**)enif_alloc_resource(group_res, sizeof(FIXGroup*));
   *grp = new_group;
   ERL_NIF_TERM grp_term = enif_make_resource(env, grp);
   enif_release_resource(grp);
   return enif_make_tuple2(env, ok_atom, enif_make_tuple2(
            env,
            group_atom,
            enif_make_tuple3(env, pres, mres, grp_term)));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_group(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM pres;
   ERL_NIF_TERM mres;
   ERL_NIF_TERM gres;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &pres, &mres, &gres, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   int32_t idx = 0;
   if (!enif_get_int(env, argv[2], &idx))
   {
      return make_error(env, FIX_FAILED, "Wrong idx.");
   }
   FIXError* error = NULL;
   pthread_rwlock_rdlock(&parser->lock);
   FIXGroup* ret_group = fix_msg_get_group(msg, group, tagNum, idx, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (!ret_group)
   {
      ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return ret;
   }
   FIXGroup** grp = (FIXGroup**)enif_alloc_resource(group_res, sizeof(FIXGroup*));
   *grp = ret_group;
   ERL_NIF_TERM grp_term = enif_make_resource(env, grp);
   enif_release_resource(grp);
   return enif_make_tuple2(env, ok_atom, enif_make_tuple2(
            env,
            group_atom,
            enif_make_tuple3(env, pres, mres, grp_term)));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM del_group(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   int32_t idx = 0;
   if (!enif_get_int(env, argv[2], &idx))
   {
      return make_error(env, FIX_FAILED, "Wrong idx.");
   }
   ERL_NIF_TERM ret = ok_atom;
   FIXError* error = NULL;
   pthread_rwlock_wrlock(&parser->lock);
   FIXErrCode err = fix_msg_del_group(msg, group, tagNum, idx, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
   }
   return ret;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM set_int32_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   if (!enif_is_number(env, argv[2]))
   {
      return make_error(env, FIX_FAILED, "Value is not a integer.");
   }
   int32_t val = 0;
   if (!enif_get_int(env, argv[2], &val))
   {
      return make_error(env, FIX_FAILED, "Value not a 32-bit integer.");
   }
   ERL_NIF_TERM ret = ok_atom;
   FIXError* error = NULL;
   pthread_rwlock_wrlock(&parser->lock);
   FIXErrCode err = fix_msg_set_int32(msg, group, tagNum, val, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
   }
   return ret;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM set_int64_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   if (!enif_is_number(env, argv[2]))
   {
      return make_error(env, FIX_FAILED, "Value is not a integer.");
   }
   int64_t val = 0;
   if (!enif_get_int64(env, argv[2], &val))
   {
      return make_error(env, FIX_FAILED, "Value not a 64-bit integer.");
   }
   ERL_NIF_TERM ret = ok_atom;
   FIXError* error = NULL;
   pthread_rwlock_wrlock(&parser->lock);
   FIXErrCode err = fix_msg_set_int64(msg, group, tagNum, val, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
   }
   return ret;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM set_double_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   if (!enif_is_number(env, argv[2]))
   {
      return make_error(env, FIX_FAILED, "Value is not a double.");
   }
   double val = 0.0;
   if (!enif_get_double(env, argv[2], &val))
   {
      int64_t lval = 0;
      enif_get_int64(env, argv[2], &lval);
      val = lval;
   }
   ERL_NIF_TERM ret = ok_atom;
   FIXError* error = NULL;
   pthread_rwlock_wrlock(&parser->lock);
   FIXErrCode err = fix_msg_set_double(msg, group, tagNum, val, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
   }
   return ret;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM set_string_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   ErlNifBinary bin;
   if (!enif_inspect_iolist_as_binary(env, argv[2], &bin))
   {
      return make_error(env, FIX_FAILED, "Value is not an iolist.");
   }
   ERL_NIF_TERM ret = ok_atom;
   FIXError* error = NULL;
   pthread_rwlock_wrlock(&parser->lock);
   FIXErrCode err = fix_msg_set_string_len(msg, group, tagNum, (char const*)bin.data, bin.size, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
   }
   return ret;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM set_char_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   if (!enif_is_number(env, argv[2]))
   {
      return make_error(env, FIX_FAILED, "Value is not a char.");
   }
   int32_t val = 0;
   enif_get_int(env, argv[2], &val);
   if (val < 32 || val > 126)
   {
      return make_error(env, FIX_FAILED, "Value is not a char.");
   }
   ERL_NIF_TERM ret = ok_atom;
   FIXError* error = NULL;
   pthread_rwlock_wrlock(&parser->lock);
   FIXErrCode err = fix_msg_set_char(msg, group, tagNum, (char)val, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
   }
   return ret;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM set_data_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   return make_error(env, FIX_FAILED, "not impemented yet");
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_int32_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   int32_t val = 0;
   FIXError* error = NULL;
   pthread_rwlock_rdlock(&parser->lock);
   FIXErrCode err = fix_msg_get_int32(msg, group, tagNum, &val, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
       ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
       fix_error_free(error);
       return ret;
   }
   return enif_make_tuple2(env, ok_atom, enif_make_int(env, val));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_int64_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   int64_t val = 0;
   FIXError* error = NULL;
   pthread_rwlock_rdlock(&parser->lock);
   FIXErrCode err = fix_msg_get_int64(msg, group, tagNum, &val, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return ret;
   }
   return enif_make_tuple2(env, ok_atom, enif_make_int64(env, val));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_double_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   double val = 0;
   FIXError* error = NULL;
   pthread_rwlock_rdlock(&parser->lock);
   FIXErrCode err = fix_msg_get_double(msg, group, tagNum, &val, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return ret;
   }
   return enif_make_tuple2(env, ok_atom, enif_make_double(env, val));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_string_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   char const* data = NULL;
   uint32_t len = 0;
   FIXError* error = NULL;
   pthread_rwlock_rdlock(&parser->lock);
   FIXErrCode err = fix_msg_get_string(msg, group, tagNum, &data, &len, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return ret;
   }
   return enif_make_tuple2(env, ok_atom, enif_make_string_len(env, data, len, ERL_NIF_LATIN1));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_char_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ERL_NIF_TERM group_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   FIXGroup* group = NULL;
   ERL_NIF_TERM res = get_parser_msg_group(env, argv[0], &parser_res, &msg_res, &group_res, &parser, &msg, &group);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t tagNum = 0;
   if (!enif_get_int(env, argv[1], &tagNum))
   {
      return make_error(env, FIX_FAILED, "Wrong tag num.");
   }
   char val;
   FIXError* error = NULL;
   pthread_rwlock_rdlock(&parser->lock);
   FIXErrCode err = fix_msg_get_char(msg, group, tagNum, &val, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (err == FIX_FAILED)
   {
      ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return ret;
   }
   return enif_make_tuple2(env, ok_atom, enif_make_int(env, val));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_data_field(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   return make_error(env, FIX_FAILED, "not impemented yet");
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM get_header(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   int32_t delimiter = 0;
   ErlNifBinary bin;
   if (!enif_inspect_binary(env, argv[0], &bin))
   {
      return make_error(env, FIX_FAILED, "Wrong delimiter.");
   }
   if (!enif_get_int(env, argv[1], &delimiter) || delimiter <= 0 || delimiter >= 255)
   {
      return make_error(env, FIX_FAILED, "Wrong binary.");
   }
   char const* beginString = NULL;
   uint32_t beginStringLen = 0;
   char const* msgType = NULL;
   uint32_t msgTypeLen = 0;
   char const* senderCompID = NULL;
   uint32_t senderCompIDLen = 0;
   char const* targetCompID = NULL;
   uint32_t targetCompIDLen = 0;
   int64_t msgSeqNum = 0;
   FIXError* error = NULL;
   if (FIX_FAILED == fix_parser_get_header((char const*)bin.data, bin.size, delimiter,
         &beginString, &beginStringLen, &msgType, &msgTypeLen, &senderCompID, &senderCompIDLen,
         &targetCompID, &targetCompIDLen, &msgSeqNum, &error))
   {
      ERL_NIF_TERM err = make_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return err;
   }
   return enif_make_tuple2(env, ok_atom,
            enif_make_tuple6(env, msg_header,
               enif_make_string_len(env, beginString, beginStringLen, ERL_NIF_LATIN1),
               enif_make_string_len(env, msgType, msgTypeLen, ERL_NIF_LATIN1),
               enif_make_string_len(env, senderCompID, senderCompIDLen, ERL_NIF_LATIN1),
               enif_make_string_len(env, targetCompID, targetCompIDLen, ERL_NIF_LATIN1),
               enif_make_int64(env, msgSeqNum)));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM msg_to_binary(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ERL_NIF_TERM msg_res;
   ParserRes* parser = NULL;
   FIXMsg* msg = NULL;
   ERL_NIF_TERM res = get_parser_msg(env, argv[0], &parser_res, &msg_res, &parser, &msg);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t delimiter = 0;
   if (!enif_get_int(env, argv[1], &delimiter) || delimiter <= 0 || delimiter >= 255)
   {
      return make_error(env, FIX_FAILED, "Wrong delimiter.");
   }
   uint32_t reqBuffLen = DEF_BINARY_SIZE;
   ErlNifBinary bin;
   if (!enif_alloc_binary(reqBuffLen, &bin))
   {
      return make_error(env, FIX_FAILED, "Unable to allocate binary.");
   }
   FIXError* error = NULL;
   pthread_rwlock_rdlock(&parser->lock);
   if (FIX_FAILED == fix_msg_to_str(msg, (char)delimiter, (char*)bin.data, bin.size, &reqBuffLen, &error))
   {
      if (reqBuffLen > bin.size) // realloc needed
      {
         if (!enif_realloc_binary(&bin, reqBuffLen))
         {
            res = make_error(env, FIX_FAILED, "Unable to reallocate binary.");
         }
         if (FIX_FAILED == fix_msg_to_str(msg, (char)delimiter, (char*)bin.data, bin.size, &reqBuffLen, &error))
         {
            res = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
            fix_error_free(error);
         }
      }
      else
      {
         res = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
         fix_error_free(error);
      }
   }
   pthread_rwlock_unlock(&parser->lock);
   if (res != ok_atom)
   {
      enif_release_binary(&bin);
      return res;
   }
   if (bin.size > reqBuffLen)
   {
      enif_realloc_binary(&bin, reqBuffLen);
   }
   ERL_NIF_TERM bin_term = enif_make_binary(env, &bin);
   enif_release_binary(&bin);
   return enif_make_tuple2(env, ok_atom, bin_term);
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ERL_NIF_TERM binary_to_msg(ErlNifEnv* env, int32_t argc, ERL_NIF_TERM const argv[])
{
   ERL_NIF_TERM parser_res;
   ParserRes* parser = NULL;
   ERL_NIF_TERM res = get_parser(env, argv[0], &parser_res, &parser);
   if (res != ok_atom)
   {
      return res;
   }
   int32_t delimiter = 0;
   if (!enif_get_int(env, argv[1], &delimiter) || delimiter <= 0 || delimiter >= 255)
   {
      return make_error(env, FIX_FAILED, "Wrong delimiter.");
   }
   ErlNifBinary bin;
   if (!enif_inspect_binary(env, argv[2], &bin))
   {
      return make_error(env, FIX_FAILED, "Wrong binary.");
   }
   char const* stop = NULL;
   FIXError* error = NULL;
   pthread_rwlock_wrlock(&parser->lock);
   FIXMsg* fix_msg = fix_parser_str_to_msg(parser->ptr, (char const*)bin.data, bin.size, delimiter, &stop, &error);
   pthread_rwlock_unlock(&parser->lock);
   if (!fix_msg)
   {
      ERL_NIF_TERM ret = make_parser_error(env, fix_error_get_code(error), fix_error_get_text(error));
      fix_error_free(error);
      return ret;
   }
   MsgRes* msg_res = (MsgRes*)enif_alloc_resource(message_res, sizeof(MsgRes));
   msg_res->msg = fix_msg;
   msg_res->lock = &parser->lock;
   ERL_NIF_TERM msg_term = enif_make_resource(env, msg_res);
   enif_release_resource(msg_res);
   uint32_t pos = stop - (char const*)bin.data + 1;
   char const* msgType = fix_msg_get_type(fix_msg);
   return enif_make_tuple3(
         env,
         ok_atom,
         enif_make_tuple3(env,
            msg_atom,
            enif_make_string(env, msgType, ERL_NIF_LATIN1),
            enif_make_tuple2(env, parser_res, msg_term)),
         enif_make_sub_binary(env, argv[2], pos, bin.size - pos));
}

/*-----------------------------------------------------------------------------------------------------------------------*/
static ErlNifFunc nif_funcs[] =
{
   {"create",            3, create           },
   {"get_version",       1, get_version      },
   {"create_msg",        2, create_msg       },
   {"add_group",         2, add_group        },
   {"get_group",         3, get_group        },
   {"del_group",         3, del_group        },
   {"set_int32_field",   3, set_int32_field  },
   {"set_int64_field",   3, set_int64_field  },
   {"set_double_field",  3, set_double_field },
   {"set_string_field",  3, set_string_field },
   {"set_char_field",    3, set_char_field   },
   {"set_data_field",    3, set_data_field   },
   {"get_int32_field",   2, get_int32_field  },
   {"get_int64_field",   2, get_int64_field  },
   {"get_double_field",  2, get_double_field },
   {"get_string_field",  2, get_string_field },
   {"get_char_field",    2, get_char_field   },
   {"get_data_field",    2, get_data_field   },
   {"get_header",        2, get_header       },
   {"msg_to_binary",     2, msg_to_binary    },
   {"binary_to_msg",     3, binary_to_msg    }
};

ERL_NIF_INIT(fix_parser, nif_funcs, load, NULL, NULL, NULL)
