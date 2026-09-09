#include "Matching_Engine.hpp"

#include <cstdint>
#include <cstdio>
#include <iostream>

#include "Order.hpp"

using Price = uint64_t;

void Matching_Engine::match_buy(Order& order) {
  // If there's a matching price, sell
  while (!sell_book.empty() && sell_book.begin()->first <= order.price &&
         order.size != 0) {
    auto best_price_it = sell_book.begin();
    Inventory& inv = best_price_it->second;

    while (!inv.order_queue.empty() && order.size > 0) {
      Order& next_to_sell = inv.order_queue.front();

      // Cleans up the possible canceled orders
      if (next_to_sell.is_canceled) {
        inv.order_queue.pop_front();
        continue;
      }

      if (order.size >= next_to_sell.size) {
        order.size -= next_to_sell.size;
        inv.quantity -= next_to_sell.size;
        inv.order_queue.pop_front();
        active_orders.erase(next_to_sell.order_id);
      } else {
        next_to_sell.size -= order.size;
        inv.quantity -= order.size;
        order.size = 0;
        return;
      }
    }
    if (inv.order_queue.empty()) {
      sell_book.erase(best_price_it);
    }
  }
  // If there's no match, creates an order on the buy_book
  if (order.size > 0) {
    auto& inv = buy_book[order.price];
    inv.order_queue.push_back(order);
    inv.quantity += order.size;
    active_orders[order.order_id] = &inv.order_queue.back();
  }
}

/*
 * Fills the order if possible, creates a sell order otherwise.
 * @param sell_book is the active list of sell orders
 * @param buy_book is the active list of buy orders
 * @param order is the order to be traded or added to a book
 */
void Matching_Engine::match_sell(Order& order) {
  // If there's a matching price, buy
  while (!buy_book.empty() && order.price <= buy_book.begin()->first &&
         order.size != 0) {
    auto best_price_it = buy_book.begin();
    Inventory& inv = best_price_it->second;

    while (!inv.order_queue.empty() && order.size > 0) {
      Order& next_to_buy = inv.order_queue.front();

      // cleans up the canceled orders
      if (next_to_buy.is_canceled) {
        inv.order_queue.pop_front();
        continue;
      }

      if (order.size >= next_to_buy.size) {
        order.size -= next_to_buy.size;
        inv.quantity -= next_to_buy.size;
        inv.order_queue.pop_front();
        active_orders.erase(next_to_buy.order_id);
      } else {
        inv.quantity -= order.size;
        next_to_buy.size -= order.size;
        order.size = 0;
        return;
      }

      if (inv.order_queue.empty()) {
        buy_book.erase(best_price_it);
      }
    }
  }
  // If there's no match, creates an order on the sell_book
  if (order.size > 0) {
    auto& inv = sell_book[order.price];
    inv.quantity += order.size;
    inv.order_queue.push_back(order);
    active_orders[order.order_id] = &inv.order_queue.back();
  }
}

void Matching_Engine::cancel_order(Order& cancel_req) {
  auto it = active_orders.find(cancel_req.order_id);

  if (it == active_orders.end()) {
    std::cerr
        << "Error, order " << cancel_req.order_id
        << "is eiter already canceled, already fulfilled or does not exist.\n"
        << std::endl;
  }

  Order* order = it->second;

  if (order->is_canceled) {
    std::cerr << "Error, order " << cancel_req.order_id
              << " is already canceled. A flaw in the logic has allowed a "
                 "canceled order to appear in the list of active orders\n"
              << std::endl;
  }
  uint32_t amount_cancelled;

  if (cancel_req.size >= order->size) {
    amount_cancelled = order->size;
    order->is_canceled = true;
    active_orders.erase(it);
  } else {
    amount_cancelled = cancel_req.size;
    order->size -= amount_cancelled;
  }

  if (cancel_req.buy) {
    buy_book[order->price].quantity -= amount_cancelled;
  } else {
    sell_book[order->price].quantity -= amount_cancelled;
  }
}

void Matching_Engine::match_order(Order& order) {
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
