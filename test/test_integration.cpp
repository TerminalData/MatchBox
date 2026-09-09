#include <gtest/gtest.h>

#include <string_view>

#include "Matching_Engine.hpp"
#include "Parser.hpp"
#include "simdjson.h"

namespace {
void process_record(std::string_view record, Matching_Engine &engine) {
  simdjson::ondemand::parser parser;
  simdjson::padded_string padded(record);
  auto document = parser.iterate(padded);
  ASSERT_FALSE(document.error());
  auto order = parse_json(document.value());
  ASSERT_TRUE(order.has_value());
  engine.match_order(*order);
}
}  // namespace

TEST(IntegrationTest, ParsesAndMatchesASequenceOfRecords) {
  Matching_Engine engine;

  process_record(
      R"({"action":"A","price":"100","size":"10","order_id":"1","side":"S"})",
      engine);
  process_record(
      R"({"action":"A","price":"101","size":"5","order_id":"2","side":"S"})",
      engine);
  process_record(
      R"({"action":"A","price":"101","size":"12","order_id":"3","side":"B"})",
      engine);

  Order consume_remaining{101, 4, 3, Order_action::Add, true};
  engine.match_order(consume_remaining);
  EXPECT_EQ(consume_remaining.size, 0);

  Order no_remaining_sell{101, 5, 1, Order_action::Add, true};
  engine.match_order(no_remaining_sell);
  EXPECT_EQ(no_remaining_sell.size, 1);
}

TEST(IntegrationTest, ParsedCancellationRemovesTheRestingOrder) {
  Matching_Engine engine;

  process_record(
      R"({"action":"A","price":"100","size":"10","order_id":"7","side":"B"})",
      engine);
  process_record(
      R"({"action":"C","price":"100","size":"10","order_id":"7","side":"B"})",
      engine);

  Order sell{100, 8, 10, Order_action::Add, false};
  engine.match_order(sell);
  EXPECT_EQ(sell.size, 10);
}
