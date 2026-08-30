#pragma once
#include <queue>

#include "Order.hpp"

/**
 * Keeps the same price orders in a timestamp based queue
 * to maintain the best price first, then oldest offer first logic.
 */
struct Inventory {
  std::queue<Order> order_age;
  int quantity;

  Inventory() { quantity = 0; }
};
