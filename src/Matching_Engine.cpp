#include "Matching_Engine.hpp"

#include <cstdint>
#include <cstdio>
#include <iostream>

#include "Inventory.hpp"
#include "Order.hpp"

using Price = uint64_t;

void Matching_Engine::match_buy(Order &order) {
  // If there's a matching price, sell
  while (!sell_book.empty() && sell_book.begin()->first <= order.price &&
         order.size != 0) {
    auto best_price_it = sell_book.begin();
    Inventory &inv = best_price_it->second;

    while (!inv.is_empty() && order.size > 0) {
      const uint32_t current_index = inv.head;
      Order &next_to_sell = pool[current_index];

      if (order.size >= next_to_sell.size) {
        const uint32_t next_index = next_to_sell.next_index;
        const uint32_t order_id = next_to_sell.order_id;
        order.size -= next_to_sell.size;
        inv.quantity -= next_to_sell.size;
        inv.head = next_index;
        if (inv.head == NULL_INDEX) {
          inv.tail = NULL_INDEX;
        } else {
          pool[inv.head].prev_index = NULL_INDEX;
        }
        active_orders.erase(order_id);
        next_to_sell.next_index = free_head;
        next_to_sell.prev_index = NULL_INDEX;
        free_head = current_index;
      } else {
        next_to_sell.size -= order.size;
        inv.quantity -= order.size;
        order.size = 0;
        return;
      }
    }
    if (inv.is_empty()) {
      sell_book.erase(best_price_it);
    }
  }
  // If there's no match, creates an order on the buy_book
  if (order.size > 0) {
    auto &inv = buy_book[order.price];
    if (free_head == NULL_INDEX) {
      std::cerr << "Error, order pool exhausted.\n";
      return;
    }

    const uint32_t new_index = free_head;
    Order &stored_order = pool[new_index];
    free_head = stored_order.next_index;
    stored_order = order;
    stored_order.next_index = NULL_INDEX;
    stored_order.prev_index = inv.tail;

    if (inv.is_empty()) {
      inv.head = new_index;
    } else {
      pool[inv.tail].next_index = new_index;
    }
    inv.tail = new_index;
    inv.quantity += stored_order.size;
    active_orders[stored_order.order_id] = new_index;
  }
}

void Matching_Engine::match_sell(Order &order) {
  // If there's a matching price, buy
  while (!buy_book.empty() && order.price <= buy_book.begin()->first &&
         order.size != 0) {
    auto best_price_it = buy_book.begin();
    Inventory &inv = best_price_it->second;

    while (!inv.is_empty() && order.size > 0) {
      const uint32_t current_index = inv.head;
      Order &next_to_buy = pool[current_index];

      if (order.size >= next_to_buy.size) {
        const uint32_t next_index = next_to_buy.next_index;
        const uint32_t order_id = next_to_buy.order_id;
        order.size -= next_to_buy.size;
        inv.quantity -= next_to_buy.size;
        inv.head = next_index;
        if (inv.head == NULL_INDEX) {
          inv.tail = NULL_INDEX;
        } else {
          pool[inv.head].prev_index = NULL_INDEX;
        }
        active_orders.erase(order_id);
        next_to_buy.next_index = free_head;
        next_to_buy.prev_index = NULL_INDEX;
        free_head = current_index;
      } else {
        inv.quantity -= order.size;
        next_to_buy.size -= order.size;
        order.size = 0;
        return;
      }
    }
    if (inv.is_empty()) {
      buy_book.erase(best_price_it);
    }
  }
  // If there's no match, creates an order on the sell_book
  if (order.size > 0) {
    auto &inv = sell_book[order.price];
    if (free_head == NULL_INDEX) {
      std::cerr << "Error, order pool exhausted.\n";
      return;
    }

    const uint32_t new_index = free_head;
    Order &stored_order = pool[new_index];
    free_head = stored_order.next_index;
    stored_order = order;
    stored_order.next_index = NULL_INDEX;
    stored_order.prev_index = inv.tail;

    if (inv.is_empty()) {
      inv.head = new_index;
    } else {
      pool[inv.tail].next_index = new_index;
    }
    inv.tail = new_index;
    inv.quantity += stored_order.size;
    active_orders[stored_order.order_id] = new_index;
  }
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
