#pragma once
#include <sys/types.h>

#include <cstdint>
#include <string>

/**
 * Basic creation of an order object.
 */
struct Order {
  uint64_t price;
  std::string action;
  int size;
  int order_id;
  uint64_t timestamp;
  bool buy;

  Order() = delete;
  Order(uint64_t p, int s, int o, uint64_t t, bool b)
      : price(p), size(s), order_id(o), timestamp(t), buy(b) {}
};
