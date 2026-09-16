#pragma once

#include <cstdint>

struct Execution {
  uint32_t taker_order_id;
  uint32_t maker_order_id;
  uint32_t size;
  uint32_t price;
};
