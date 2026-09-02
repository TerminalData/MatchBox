#include <gtest/gtest.h>

#include <cstdint>

#include "Matching_Engine.hpp"
using Price = uint64_t;
class MatchingEngineTest : public ::testing::Test {
 protected:
  std::map<Price, Inventory, std::less<Price>> sell_book;
  std::map<Price, Inventory, std::greater<Price>> buy_book;
};

// 1. Verify resting order insertion when books cannot cross
TEST_F(MatchingEngineTest, PlacesRestingOrdersWithoutCrossing) {
  Order b1{100, "A", 10, 1, 0, true};
  Order s1{105, "A", 10, 2, 0, false};

  Matching_Engine::match_order(sell_book, buy_book, b1);
  Matching_Engine::match_order(sell_book, buy_book, s1);

  ASSERT_EQ(buy_book.size(), 1);
  EXPECT_EQ(buy_book[100].quantity, 10);
  EXPECT_EQ(b1.size, 10);  // Not consumed

  ASSERT_EQ(sell_book.size(), 1);
  EXPECT_EQ(sell_book[105].quantity, 10);
  EXPECT_EQ(s1.size, 10);  // Not consumed
}

// 2. Verify partial fills and correct reduction of volume
TEST_F(MatchingEngineTest, HandlesPartialFillsCorrectly) {
  // Resting ask: 10 units at $100
  Order s1{100, "A", 10, 1, 0, false};
  Matching_Engine::match_order(sell_book, buy_book, s1);

  // Incoming bid: 4 units at $100
  Order b1{100, "A", 4, 2, 0, true};
  Matching_Engine::match_order(sell_book, buy_book, b1);

  // Incoming order should be fully consumed
  EXPECT_EQ(b1.size, 0);

  // Resting level should reflect remaining size
  ASSERT_NE(sell_book.find(100), sell_book.end());
  EXPECT_EQ(sell_book[100].quantity, 6);
  EXPECT_EQ(sell_book[100].order_queue.front().size, 6);
}

// 3. Verify price levels are cleanly erased when completely exhausted
TEST_F(MatchingEngineTest, ErasesExhaustedPriceLevel) {
  Order s1{100, "A", 5, 1, 0, false};
  Matching_Engine::match_order(sell_book, buy_book, s1);

  Order b1{100, "A", 5, 2, 0, true};
  Matching_Engine::match_order(sell_book, buy_book, b1);

  EXPECT_EQ(b1.size, 0);
  EXPECT_TRUE(sell_book.empty());
  EXPECT_EQ(sell_book.find(100), sell_book.end());
}

// 4. Verify FIFO priority across multiple orders at the same price level
TEST_F(MatchingEngineTest, RespectsFifoOrderPriority) {
  Order s1{100, "A", 5, 101, 0, false};
  Order s2{100, "A", 5, 102, 0, false};
  Matching_Engine::match_order(sell_book, buy_book, s1);
  Matching_Engine::match_order(sell_book, buy_book, s2);

  // Aggressive buy consumes s1 fully, partially absorbs s2
  Order b1{100, "A", 7, 201, 0, true};
  Matching_Engine::match_order(sell_book, buy_book, b1);

  EXPECT_EQ(b1.size, 0);
  ASSERT_EQ(sell_book[100].order_queue.size(), 1);

  // s1 must be gone; the remaining order at front must be s2
  const Order& remaining = sell_book[100].order_queue.front();
  EXPECT_EQ(remaining.order_id, 102);
  EXPECT_EQ(remaining.size, 3);
  EXPECT_EQ(sell_book[100].quantity, 3);
}

// 5. Verify aggressive orders sweeping through multiple price levels
TEST_F(MatchingEngineTest, SweepsMultiplePriceLevelsAndRestsRemainder) {
  Order s1{100, "A", 5, 1, 0, false};
  Order s2{101, "A", 5, 2, 0, false};
  Matching_Engine::match_order(sell_book, buy_book, s1);
  Matching_Engine::match_order(sell_book, buy_book, s2);

  // Buy 15 units willing to pay up to $102
  Order b1{102, "A", 15, 3, 0, true};
  Matching_Engine::match_order(sell_book, buy_book, b1);

  // Both sell levels should be cleared
  EXPECT_TRUE(sell_book.empty());

  // Remaining 5 units from the buy order must rest on the buy book
  ASSERT_EQ(buy_book.size(), 1);
  EXPECT_EQ(buy_book[102].quantity, 5);
  EXPECT_EQ(buy_book[102].order_queue.front().size, 5);
}

// 6. Verify cancellation logic on resting orders
TEST_F(MatchingEngineTest, CancelsRestingOrderCorrectly) {
  Order s1{100, "A", 10, 1, 0, false};
  Matching_Engine::match_order(sell_book, buy_book, s1);

  // Full cancel request
  Order c1{100, "C", 10, 1, 0, false};
  Matching_Engine::match_order(sell_book, buy_book, c1);

  EXPECT_TRUE(sell_book.empty());
  EXPECT_EQ(sell_book.find(100), sell_book.end());
}
