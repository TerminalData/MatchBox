#pragma once
#include <queue>

#include "Order.hpp"

struct Inventory {
  std::queue<Order> order_age;
  int quantity;

  Inventory() { quantity = 0; }
};
