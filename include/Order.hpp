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
  bool buy;

  Order() = delete;
  Order(uint64_t p, std::string a, int s, int o, bool b)
      : price(p), action(a), size(s), order_id(o), buy(b) {}
};
