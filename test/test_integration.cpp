#include <gtest/gtest.h>

#include <cstdint>
#include <string_view>

#include "Inventory.hpp"
#include "Matching_Engine.hpp"
#include "Parser.hpp"
#include "simdjson.h"

namespace {
using SellBook = std::map<uint64_t, Inventory, std::less<uint64_t>>;
using BuyBook = std::map<uint64_t, Inventory, std::greater<uint64_t>>;

void process_record(std::string_view record, SellBook &sell_book,
                    BuyBook &buy_book) {
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(record);
  auto document = parser.iterate(padded);
  ASSERT_FALSE(document.error());
  auto order = parse_json(document.value());
  ASSERT_TRUE(order.has_value());
  Matching_Engine::match_order(sell_book, buy_book, *order);
}
}  // namespace

TEST(IntegrationTest, ParsesAndMatchesASequenceOfRecords) {
  SellBook sell_book;
  BuyBook buy_book;

  process_record(
      R"({"action":"A","price":"100","size":"10","order_id":"1","side":"S"})",
      sell_book, buy_book);
  process_record(
      R"({"action":"A","price":"101","size":"5","order_id":"2","side":"S"})",
      sell_book, buy_book);
  process_record(
      R"({"action":"A","price":"101","size":"12","order_id":"3","side":"B"})",
      sell_book, buy_book);

  ASSERT_EQ(sell_book.size(), 1);
  EXPECT_EQ(sell_book.at(101).quantity, 3);
  EXPECT_TRUE(buy_book.empty());
}

TEST(IntegrationTest, ParsedCancellationRemovesTheRestingOrder) {
  SellBook sell_book;
  BuyBook buy_book;

  process_record(
      R"({"action":"A","price":"100","size":"10","order_id":"7","side":"B"})",
      sell_book, buy_book);
  process_record(
      R"({"action":"C","price":"100","size":"10","order_id":"7","side":"B"})",
      sell_book, buy_book);

  EXPECT_TRUE(buy_book.empty());
  EXPECT_TRUE(sell_book.empty());
}
