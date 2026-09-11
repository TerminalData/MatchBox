#pragma once
#include <cstdint>

#include "Order.hpp"

/**
 * Keeps the FIFO order of Orders via a linked list.
 */
struct Inventory {
  uint32_t head = NULL_INDEX;
  uint32_t tail = NULL_INDEX;

  uint64_t quantity = 0;

  inline bool is_empty() const { return head == NULL_INDEX; }
};
