#pragma once

#include <cstdint>
#include <cstdio>
#include <map>
#include <unordered_map>

#include "Inventory.hpp"
#include "Order.hpp"

class Matching_Engine {
 private:
  using Price = uint64_t;

  std::map<Price, Inventory, std::less<Price>> sell_book;
  std::map<Price, Inventory, std::greater<Price>> buy_book;
  std::unordered_map<uint32_t, Order*> active_orders;

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
  Matching_Engine() = default;

  /*
   * Manages the orders and redistributes them according to their
   * action and side.
   *
   * @param order is the order to be traded or added to a book
   */
  void match_order(Order& order);
};
