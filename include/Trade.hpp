#pragma once

#include <cstdint>

struct Trade {
  uint64_t price;
  uint32_t size;
  bool buy;

  Trade() = default;
  Trade(uint64_t p, uint32_t s, bool b) : price(p), size(s), buy(b) {}
};
