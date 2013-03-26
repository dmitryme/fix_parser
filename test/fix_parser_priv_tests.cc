/**
 * @file   fix_parser_tests.c
 * @author Dmitry S. Melnikov, dmitryme@gmail.com
 * @date   Created on: 08/03/2012 02:41:02 PM
 */

#include <fix_parser.h>
#include <fix_parser_priv.h>

#include <gtest/gtest.h>

TEST(FixParserPrivTests, CreateParserTest)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 5, 2, 5};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->flags, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser->page != NULL);
   ASSERT_TRUE(parser->page->next != NULL);
   ASSERT_TRUE(parser->page->next->next == NULL);
   ASSERT_EQ(parser->used_pages, 0U);
   ASSERT_EQ(parser->attrs.maxPages, 5U);
   ASSERT_EQ(parser->attrs.pageSize, 512U);
   ASSERT_TRUE(parser->group != NULL);
   ASSERT_EQ(parser->used_groups, 0U);
   ASSERT_EQ(parser->attrs.maxGroups, 5U);

   fix_parser_free(parser);
}

TEST(FixParserPrivTests, SetErrorTest)
{
   FIXError* error = NULL;
   error = fix_error_create(FIX_ERROR_NO_MORE_PAGES, "No more pages available");

   ASSERT_EQ(error->code, FIX_ERROR_NO_MORE_PAGES);
   ASSERT_STREQ(error->text, "No more pages available");

   fix_error_free(error);
}

TEST(FixParserPrivTests, GetFreePageTest)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);

   FIXPage* nextp = parser->page->next;
   FIXPage* p = fix_parser_alloc_page(parser, 0, &error);

   ASSERT_TRUE(p->next == NULL);
   ASSERT_EQ(parser->page, nextp);
   ASSERT_EQ(parser->used_pages, 1U);

   FIXPage* p1 = fix_parser_alloc_page(parser, 0, &error);
   ASSERT_EQ(p1, nextp);
   ASSERT_TRUE(p1->next == NULL);
   ASSERT_EQ(parser->used_pages, 2U);

   FIXPage* p2 = fix_parser_alloc_page(parser, 0, &error);
   ASSERT_TRUE(parser->page == NULL);
   ASSERT_NE(p2, nextp);
   ASSERT_TRUE(p1->next == NULL);
   ASSERT_EQ(parser->used_pages, 3U);

   fix_parser_free_page(parser, p);

   ASSERT_EQ(parser->page, p);

   fix_parser_free_page(parser, p2);
   ASSERT_EQ(parser->page, p2);
   ASSERT_EQ(parser->page->next, p);

   fix_parser_free_page(parser, p1);
   ASSERT_EQ(parser->page, p1);
   ASSERT_EQ(parser->page->next, p2);

   ASSERT_EQ(parser->used_pages, 0U);

   fix_parser_free(parser);
}

TEST(FixParserPrivTests, MaxPagesTest)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 2, 2, 0};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);

   FIXPage* nextp = parser->page->next;
   ASSERT_TRUE(parser->page != NULL);

   FIXPage* p = fix_parser_alloc_page(parser, 0, &error);
   ASSERT_TRUE(p->next == NULL);
   ASSERT_EQ(parser->page, nextp);
   ASSERT_EQ(parser->used_pages, 1U);

   FIXPage* p1 = fix_parser_alloc_page(parser, 0, &error);
   ASSERT_EQ(p1, nextp);
   ASSERT_TRUE(p1->next == NULL);
   ASSERT_EQ(parser->used_pages, 2U);

   FIXPage* p2 = fix_parser_alloc_page(parser, 0, &error);
   ASSERT_TRUE(p2 == NULL);
   ASSERT_EQ(error->code, FIX_ERROR_NO_MORE_PAGES);
   ASSERT_TRUE(parser->page == NULL);
   ASSERT_EQ(parser->used_pages, 2U);

   fix_parser_free_page(parser, p);

   ASSERT_EQ(parser->page, p);
   ASSERT_EQ(parser->used_pages, 1U);

   p = fix_parser_alloc_page(parser, 0, &error);

   ASSERT_TRUE(p->next == NULL);
   ASSERT_TRUE(parser->page == NULL);
   ASSERT_EQ(parser->used_pages, 2U);

   fix_parser_free(parser);
}

TEST(FixParserPrivTests, MaxGroupsTest)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 2};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);

   FIXGroup* nextg = parser->group->next;
   ASSERT_TRUE(parser->group != NULL);

   FIXGroup* g = fix_parser_alloc_group(parser, &error);
   ASSERT_TRUE(g->next == NULL);
   ASSERT_EQ(parser->group, nextg);
   ASSERT_EQ(parser->used_groups, 1U);

   FIXGroup* g1 = fix_parser_alloc_group(parser, &error);
   ASSERT_EQ(g1, nextg);
   ASSERT_TRUE(g1->next == NULL);
   ASSERT_EQ(parser->used_groups, 2U);

   FIXGroup* p2 = fix_parser_alloc_group(parser, &error);
   ASSERT_TRUE(p2 == NULL);
   ASSERT_EQ(error->code, FIX_ERROR_NO_MORE_GROUPS);
   ASSERT_TRUE(parser->group == NULL);
   ASSERT_EQ(parser->used_groups, 2U);

   fix_parser_free_group(parser, g);

   g = fix_parser_alloc_group(parser, &error);
   ASSERT_TRUE(g->next == NULL);
   ASSERT_TRUE(parser->group == NULL);
   ASSERT_EQ(parser->used_groups, 2U);

   fix_parser_free(parser);
}

TEST(FixParserPrivTests, MaxPageSizeTest)
{
   {
      FIXError* error = NULL;
      FIXParserAttrs attrs = {512, 0, 1, 0, 2, 0};
      FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
      ASSERT_TRUE(parser != NULL);

      FIXPage* p = fix_parser_alloc_page(parser, 0, &error);
      ASSERT_TRUE(p->next == NULL);
      ASSERT_EQ(parser->used_pages, 1U);
      ASSERT_EQ(p->size, 512U);

      FIXPage* p1 = fix_parser_alloc_page(parser, 1024, &error);
      ASSERT_TRUE(p1->next == NULL);
      ASSERT_EQ(parser->used_pages, 2U);
      ASSERT_EQ(p1->size, 1024U);

      FIXPage* p2 = fix_parser_alloc_page(parser, 10240, &error);
      ASSERT_TRUE(p2->next == NULL);
      ASSERT_EQ(parser->used_pages, 3U);
      ASSERT_EQ(p2->size, 10240U);

      fix_parser_free(parser);
   }
   {
      FIXError* error = NULL;
      FIXParserAttrs attrs = {512, 512, 1, 0, 2, 0};
      FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
      ASSERT_TRUE(parser != NULL);

      FIXPage* p = fix_parser_alloc_page(parser, 0, &error);
      ASSERT_TRUE(p != NULL);
      ASSERT_EQ(parser->used_pages, 1U);
      ASSERT_EQ(p->size, 512U);

      FIXPage* p1 = fix_parser_alloc_page(parser, 500, &error);
      ASSERT_TRUE(p1 != NULL);
      ASSERT_EQ(parser->used_pages, 2U);
      ASSERT_EQ(p->size, 512U);

      FIXPage* p2 = fix_parser_alloc_page(parser, 513, &error);
      ASSERT_TRUE(p2 == NULL);
      ASSERT_EQ(error->code, FIX_ERROR_TOO_BIG_PAGE);
      ASSERT_EQ(parser->used_pages, 2U);

      fix_parser_free(parser);
   }
}

TEST(FixParserPrivTests, ParseMandatoryField)
{
   FIXError* error = NULL;
   FIXParserAttrs attrs = {512, 0, 2, 5, 2, 5};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL, &error);
   ASSERT_TRUE(parser != NULL);
   char const* dbegin = NULL;
   char const* dend = NULL;
   {
      char const data[] = "8=FIX4.4\00113=100\001";
      ASSERT_EQ(8, fix_parser_parse_mandatory_field(data, strlen(data), '\001', &dbegin, &dend, &error));
      ASSERT_TRUE(!strncmp("FIX4.4", dbegin, dend - dbegin));
   }
   {
      char const data[] = "A=FIX4.4\0013=100\001";
      ASSERT_EQ(FIX_FAILED, fix_parser_parse_mandatory_field(data, strlen(data), '\001', &dbegin, &dend, &error));
      ASSERT_EQ(FIX_ERROR_INVALID_ARGUMENT, fix_error_get_code(error));
   }
   {
      char const data[] = "8-FIX4.4\001";
      ASSERT_EQ(FIX_FAILED, fix_parser_parse_mandatory_field(data, strlen(data), '\001', &dbegin, &dend, &error));
      ASSERT_EQ(FIX_ERROR_INVALID_ARGUMENT, fix_error_get_code(error));
   }
   {
      char const data[] = "8=FIX4.4|";
      ASSERT_EQ(FIX_FAILED, fix_parser_parse_mandatory_field(data, strlen(data), '\001', &dbegin, &dend, &error));
      ASSERT_EQ(FIX_ERROR_NO_MORE_DATA, fix_error_get_code(error));
   }
   {
      char const data[] = "8";
      ASSERT_EQ(FIX_FAILED, fix_parser_parse_mandatory_field(data, strlen(data), '\001', &dbegin, &dend, &error));
      ASSERT_EQ(FIX_ERROR_NO_MORE_DATA, fix_error_get_code(error));
   }
   {
      char const data[] = "8=";
      ASSERT_EQ(FIX_FAILED, fix_parser_parse_mandatory_field(data, strlen(data), '\001', &dbegin, &dend, &error));
      ASSERT_EQ(FIX_ERROR_NO_MORE_DATA, fix_error_get_code(error));
   }
   {
      char const data[] = "8=F";
      ASSERT_EQ(FIX_FAILED, fix_parser_parse_mandatory_field(data, strlen(data), '\001', &dbegin, &dend, &error));
      ASSERT_EQ(FIX_ERROR_NO_MORE_DATA, fix_error_get_code(error));
   }

   fix_parser_free(parser);
}
