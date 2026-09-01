#pragma once
#include <list>

#include "Order.hpp"

/**
 * Keeps the same price orders in a timestamp based queue
 * to maintain the best price first, then oldest offer first logic.
 */
struct Inventory {
  using Order_Id = int;
  std::list<Order> order_queue;
  int quantity;

  Inventory() { quantity = 0; }
};
