#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <map>
#include <unordered_map>
#include <vector>

#include "Inventory.hpp"
#include "Order.hpp"

class Matching_Engine {
 private:
  using Price = uint64_t;
  using Id = uint32_t;
  using Index = uint32_t;
  std::vector<Order> pool;
  uint32_t free_head = 0;

  std::map<Price, Inventory, std::less<Price>> sell_book;
  std::map<Price, Inventory, std::greater<Price>> buy_book;
  std::unordered_map<Id, Index> active_orders;

  /*
   * Fills the order if possible, creates a buy order otherwise.
   * @param order is the order to be traded or added to a book
   */
  void match_buy(Order& order);

  /*
   * Fills the order if possible, creates a sell order otherwise.
   * @param order is the order to be traded or added to a book
   */
  void match_sell(Order& order);

  /*
   * Finds the correct order in the active_orders book, and switches it
   * is_canceled field to true (tombstoning).
   *
   * @param order is the order to be canceled.
   */
  void cancel_order(Order& cancel_req);

 public:
  explicit Matching_Engine(std::size_t pool_size = 2000000) {
    pool.resize(pool_size);

    for (uint32_t i = 0; i < pool_size - 1; ++i) {
      pool[i].next_index = i + 1;
    }
    pool.back().next_index = NULL_INDEX;
  }

  /*
   * Manages the orders and redistributes them according to their
   * action and side.
   *
   * @param order is the order to be traded or added to a book
   */
  void match_order(Order& order);
};
