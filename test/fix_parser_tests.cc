/* @file   fix_parser_tests.c
   @author Dmitry S. Melnikov, dmitryme@gmail.com
   @date   Created on: 08/03/2012 02:41:02 PM
*/

#include <fix_parser.h>
#include <fix_parser_priv.h>

#include <gtest/gtest.h>

TEST(FixParserTests, CreateParserTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 5, 2, 5};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);
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

TEST(FixParserTests, SetErrorParserTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 5, 2, 5};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);

   fix_error_set(&parser->error, FIX_ERROR_NO_MORE_PAGES, "No more pages available");

   ASSERT_EQ(parser->error.code, FIX_ERROR_NO_MORE_PAGES);
   ASSERT_TRUE(!strcmp(parser->error.text, "No more pages available"));

   fix_error_reset(&parser->error);

   ASSERT_EQ(parser->error.code, 0);
   ASSERT_TRUE(!strcmp(parser->error.text, ""));

   fix_parser_free(parser);
}

TEST(FixParserTests, GetFreePageTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 0};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   FIXPage* nextp = parser->page->next;
   FIXPage* p = fix_parser_alloc_page(parser, 0);

   ASSERT_TRUE(p->next == NULL);
   ASSERT_EQ(parser->page, nextp);
   ASSERT_EQ(parser->used_pages, 1U);

   FIXPage* p1 = fix_parser_alloc_page(parser, 0);
   ASSERT_EQ(p1, nextp);
   ASSERT_TRUE(p1->next == NULL);
   ASSERT_EQ(parser->used_pages, 2U);

   FIXPage* p2 = fix_parser_alloc_page(parser, 0);
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

TEST(FixParserTests, MaxPagesTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 2, 2, 0};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   FIXPage* nextp = parser->page->next;
   ASSERT_TRUE(parser->page != NULL);

   FIXPage* p = fix_parser_alloc_page(parser, 0);
   ASSERT_TRUE(p->next == NULL);
   ASSERT_EQ(parser->page, nextp);
   ASSERT_EQ(parser->used_pages, 1U);

   FIXPage* p1 = fix_parser_alloc_page(parser, 0);
   ASSERT_EQ(p1, nextp);
   ASSERT_TRUE(p1->next == NULL);
   ASSERT_EQ(parser->used_pages, 2U);

   FIXPage* p2 = fix_parser_alloc_page(parser, 0);
   ASSERT_TRUE(p2 == NULL);
   ASSERT_EQ(parser->error.code, FIX_ERROR_NO_MORE_PAGES);
   ASSERT_TRUE(parser->page == NULL);
   ASSERT_EQ(parser->used_pages, 2U);

   fix_parser_free_page(parser, p);

   ASSERT_EQ(parser->page, p);
   ASSERT_EQ(parser->used_pages, 1U);

   p = fix_parser_alloc_page(parser, 0);

   ASSERT_TRUE(p->next == NULL);
   ASSERT_TRUE(parser->page == NULL);
   ASSERT_EQ(parser->used_pages, 2U);

   fix_parser_free(parser);
}

TEST(FixParserTests, MaxGroupsTest)
{
   FIXParserAttrs attrs = {512, 0, 2, 0, 2, 2};
   FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL);
   ASSERT_TRUE(parser != NULL);
   ASSERT_EQ(parser->error.code, 0);

   FIXGroup* nextg = parser->group->next;
   ASSERT_TRUE(parser->group != NULL);

   FIXGroup* g = fix_parser_alloc_group(parser);
   ASSERT_TRUE(g->next == NULL);
   ASSERT_EQ(parser->group, nextg);
   ASSERT_EQ(parser->used_groups, 1U);

   FIXGroup* g1 = fix_parser_alloc_group(parser);
   ASSERT_EQ(g1, nextg);
   ASSERT_TRUE(g1->next == NULL);
   ASSERT_EQ(parser->used_groups, 2U);

   FIXGroup* p2 = fix_parser_alloc_group(parser);
   ASSERT_TRUE(p2 == NULL);
   ASSERT_EQ(parser->error.code, FIX_ERROR_NO_MORE_GROUPS);
   ASSERT_TRUE(parser->group == NULL);
   ASSERT_EQ(parser->used_groups, 2U);

   fix_parser_free_group(parser, g);

   g = fix_parser_alloc_group(parser);
   ASSERT_TRUE(g->next == NULL);
   ASSERT_TRUE(parser->group == NULL);
   ASSERT_EQ(parser->used_groups, 2U);

   fix_parser_free(parser);
}

TEST(FixParserTests, MaxPageSizeTest)
{
   {
      FIXParserAttrs attrs = {512, 0, 1, 0, 2, 0};
      FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL);
      ASSERT_TRUE(parser != NULL);
      ASSERT_EQ(parser->error.code, 0);

      FIXPage* p = fix_parser_alloc_page(parser, 0);
      ASSERT_TRUE(p->next == NULL);
      ASSERT_EQ(parser->used_pages, 1U);
      ASSERT_EQ(p->size, 512U);

      FIXPage* p1 = fix_parser_alloc_page(parser, 1024);
      ASSERT_TRUE(p1->next == NULL);
      ASSERT_EQ(parser->used_pages, 2U);
      ASSERT_EQ(p1->size, 1024U);

      FIXPage* p2 = fix_parser_alloc_page(parser, 10240);
      ASSERT_TRUE(p2->next == NULL);
      ASSERT_EQ(parser->used_pages, 3U);
      ASSERT_EQ(p2->size, 10240U);

      fix_parser_free(parser);
   }
   {
      FIXParserAttrs attrs = {512, 512, 1, 0, 2, 0};
      FIXParser* parser = fix_parser_create("fix_descr/fix.4.4.xml", &attrs, PARSER_FLAG_CHECK_ALL);
      ASSERT_TRUE(parser != NULL);
      ASSERT_EQ(parser->error.code, 0);

      FIXPage* p = fix_parser_alloc_page(parser, 0);
      ASSERT_TRUE(p != NULL);
      ASSERT_EQ(parser->used_pages, 1U);
      ASSERT_EQ(p->size, 512U);

      FIXPage* p1 = fix_parser_alloc_page(parser, 500);
      ASSERT_TRUE(p1 != NULL);
      ASSERT_EQ(parser->used_pages, 2U);
      ASSERT_EQ(p->size, 512U);

      FIXPage* p2 = fix_parser_alloc_page(parser, 513);
      ASSERT_TRUE(p2 == NULL);
      ASSERT_EQ(parser->error.code, FIX_ERROR_TOO_BIG_PAGE);
      ASSERT_EQ(parser->used_pages, 2U);

      fix_parser_free(parser);
   }
}
