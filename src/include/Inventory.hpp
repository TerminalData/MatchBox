
#include <queue>

#include "Order.hpp"

struct Inventory {
  std::queue<Order> price_level;
  int quantity;
};
