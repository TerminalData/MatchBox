#include <gtest/gtest.h>

#include <cstdint>
#include <map>

#include "Inventory.hpp"
#include "Matching_Engine.hpp"
#include "Order.hpp"

namespace {
template <typename Compare>
void expect_book_invariants(
    const std::map<uint64_t, Inventory, Compare> &book) {
  for (const auto &[price, inventory] : book) {
    (void)price;
    ASSERT_FALSE(inventory.order_queue.empty());

    uint64_t calculated_quantity = 0;
    for (const auto &order : inventory.order_queue) {
      EXPECT_GT(order.price, 0);
      EXPECT_GT(order.order_id, 0);
      EXPECT_GT(order.size, 0);
      calculated_quantity += order.size;
    }
    EXPECT_EQ(inventory.quantity, calculated_quantity);
  }
}
}  // namespace

TEST(InvariantTest, QuantitiesRemainConsistentAcrossMatchesAndCancels) {
  std::map<uint64_t, Inventory, std::less<uint64_t>> sell_book;
  std::map<uint64_t, Inventory, std::greater<uint64_t>> buy_book;

  Order sell_one{100, 1, 10, Order_action::Add, false};
  Order sell_two{101, 2, 8, Order_action::Add, false};
  Order buy_one{101, 3, 6, Order_action::Add, true};
  Order cancel_two{101, 2, 3, Order_action::Cancel, false};
  Order buy_two{100, 4, 20, Order_action::Add, true};

  Matching_Engine::match_order(sell_book, buy_book, sell_one);
  Matching_Engine::match_order(sell_book, buy_book, sell_two);
  Matching_Engine::match_order(sell_book, buy_book, buy_one);
  expect_book_invariants(sell_book);
  expect_book_invariants(buy_book);

  Matching_Engine::match_order(sell_book, buy_book, cancel_two);
  expect_book_invariants(sell_book);
  expect_book_invariants(buy_book);

  Matching_Engine::match_order(sell_book, buy_book, buy_two);
  expect_book_invariants(sell_book);
  expect_book_invariants(buy_book);
  ASSERT_EQ(sell_book.size(), 1);
  EXPECT_EQ(sell_book.at(101).quantity, 5);
  ASSERT_EQ(buy_book.size(), 1);
  EXPECT_EQ(buy_book.at(100).quantity, 16);
}

TEST(InvariantTest, FIFOOrdersRemainAtTheirPriceLevel) {
  std::map<uint64_t, Inventory, std::less<uint64_t>> sell_book;
  std::map<uint64_t, Inventory, std::greater<uint64_t>> buy_book;

  Order first{100, 10, 3, Order_action::Add, false};
  Order second{100, 11, 4, Order_action::Add, false};
  Matching_Engine::match_order(sell_book, buy_book, first);
  Matching_Engine::match_order(sell_book, buy_book, second);

  const auto &queue = sell_book.at(100).order_queue;
  ASSERT_EQ(queue.size(), 2);
  EXPECT_EQ(queue.front().order_id, 10);
  EXPECT_EQ(queue.back().order_id, 11);
  expect_book_invariants(sell_book);
}
