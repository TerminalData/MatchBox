#pragma once
#include <sys/types.h>

#include <cstdint>

enum class Order_action : uint8_t { Add = 0, Cancel = 1 };

/**
 * Basic creation of an order object.
 */
struct Order {
  uint64_t price;
  Order_action action;
  int size;
  int order_id;
  bool buy;

  Order() = delete;
  Order(uint64_t p, Order_action(a), int s, int o, bool b)
      : price(p), action(a), size(s), order_id(o), buy(b) {}
};
