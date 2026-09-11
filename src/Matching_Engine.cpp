#include "Matching_Engine.hpp"

#include <cstdint>
#include <cstdio>
#include <iostream>

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

      // Cleans up the possible canceled orders
      if (next_to_sell.is_canceled) {
        const uint32_t next_index = next_to_sell.next_index;
        inv.head = next_index;
        if (inv.head == NULL_INDEX) {
          inv.tail = NULL_INDEX;
        }
        next_to_sell.next_index = free_head;
        free_head = current_index;
        continue;
      }

      if (order.size >= next_to_sell.size) {
        const uint32_t next_index = next_to_sell.next_index;
        const uint32_t order_id = next_to_sell.order_id;
        order.size -= next_to_sell.size;
        inv.quantity -= next_to_sell.size;
        inv.head = next_index;
        if (inv.head == NULL_INDEX) {
          inv.tail = NULL_INDEX;
        }
        active_orders.erase(order_id);
        next_to_sell.next_index = free_head;
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

      // cleans up the canceled orders
      if (next_to_buy.is_canceled) {
        const uint32_t next_index = next_to_buy.next_index;
        inv.head = next_index;
        if (inv.head == NULL_INDEX) {
          inv.tail = NULL_INDEX;
        }
        next_to_buy.next_index = free_head;
        free_head = current_index;
        continue;
      }

      if (order.size >= next_to_buy.size) {
        const uint32_t next_index = next_to_buy.next_index;
        const uint32_t order_id = next_to_buy.order_id;
        order.size -= next_to_buy.size;
        inv.quantity -= next_to_buy.size;
        inv.head = next_index;
        if (inv.head == NULL_INDEX) {
          inv.tail = NULL_INDEX;
        }
        active_orders.erase(order_id);
        next_to_buy.next_index = free_head;
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

  Order &order = pool[it->second];

  if (order.is_canceled) {
    std::cerr << "Error, order " << cancel_req.order_id
              << " is already canceled. A flaw in the logic has allowed a "
                 "canceled order to appear in the list of active orders\n"
              << std::endl;
    return;
  }
  uint32_t amount_cancelled;

  if (cancel_req.size >= order.size) {
    amount_cancelled = order.size;
    order.is_canceled = true;
    active_orders.erase(it);
  } else {
    amount_cancelled = cancel_req.size;
    order.size -= amount_cancelled;
  }

  if (order.buy) {
    buy_book[order.price].quantity -= amount_cancelled;
  } else {
    sell_book[order.price].quantity -= amount_cancelled;
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
