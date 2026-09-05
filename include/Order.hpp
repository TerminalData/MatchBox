#pragma once
#include <sys/types.h>

#include <cstdint>

enum class Order_action : uint8_t { Add = 0, Cancel = 1 };

/**
 * Basic creation of an order object. currently caping size at uint16_t which
 * represents a max order size of 65,535 shares. order_id in benchmark file are
 * always 9 or 10digits long, uint32_t fits that range.
 */
struct Order {
  uint64_t price;
  Order_action action;
  uint16_t size;
  uint32_t order_id;
  bool buy;

  Order() = delete;
  Order(uint64_t p, Order_action(a), uint16_t s, uint32_t o, bool b)
      : price(p), action(a), size(s), order_id(o), buy(b) {}
};
