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
  // 8 byte
  uint64_t price;

  // 4 byte
  uint32_t order_id;

  // 2 byte
  uint32_t size;

  // 1 byte
  Order_action action;
  bool buy;

  Order() = delete;
  Order(uint64_t p, uint32_t o, uint32_t s, Order_action a, bool b)
      : price(p), order_id(o), size(s), action(a), buy(b) {}
};
