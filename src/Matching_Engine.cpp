#include "Matching_Engine.hpp"

#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>

#include "Inventory.hpp"
#include "Order.hpp"

using Price = uint64_t;

void Matching_Engine::match_buy(Order &order) {
  match_against(order, buy_book, sell_book, std::less_equal<uint64_t>());
}

void Matching_Engine::match_sell(Order &order) {
  match_against(order, sell_book, buy_book, std::greater_equal<uint64_t>());
}

void Matching_Engine::cancel_order(Order &cancel_req) {
  auto it = active_orders.find(cancel_req.order_id);

  if (it == active_orders.end()) {
    std::cerr
        << "Error, order " << cancel_req.order_id
        << "is eiter already canceled, already fulfilled or does not exist.\n"
        << std::endl;
    return;
  }

  const uint32_t current_index = it->second;
  Order &order = pool[current_index];
  const uint32_t amount_cancelled =
      cancel_req.size < order.size ? cancel_req.size : order.size;

  auto unlink_order = [&](auto &book) {
    auto price_it = book.find(order.price);
    if (price_it == book.end()) {
      return;
    }

    Inventory &inv = price_it->second;
    const uint32_t prev_index = order.prev_index;
    const uint32_t next_index = order.next_index;

    if (prev_index == NULL_INDEX) {
      inv.head = next_index;
    } else {
      pool[prev_index].next_index = next_index;
    }

    if (next_index == NULL_INDEX) {
      inv.tail = prev_index;
    } else {
      pool[next_index].prev_index = prev_index;
    }

    inv.quantity -= amount_cancelled;
    if (inv.is_empty()) {
      book.erase(price_it);
    }
  };

  if (cancel_req.size >= order.size) {
    if (order.buy) {
      unlink_order(buy_book);
    } else {
      unlink_order(sell_book);
    }

    active_orders.erase(it);
    order.next_index = free_head;
    order.prev_index = NULL_INDEX;
    free_head = current_index;
  } else {
    order.size -= amount_cancelled;
    if (order.buy) {
      buy_book.find(order.price)->second.quantity -= amount_cancelled;
    } else {
      sell_book.find(order.price)->second.quantity -= amount_cancelled;
    }
  }
}

void Matching_Engine::match_order(Order &order) {
  if (order.action == Order_action::Cancel) {
    cancel_order(order);
  } else if (order.action == Order_action::Add) {
    if (order.buy) {
      match_buy(order);
    } else {
      match_sell(order);
    }
  } else {
    std::cerr << "Error, order action not recognized." << std::endl;
    exit(EXIT_FAILURE);
  }
}
