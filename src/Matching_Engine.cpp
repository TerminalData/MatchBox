#include "Matching_Engine.hpp"

#include <cstdint>
#include <cstdio>
#include <iostream>

namespace Matching_Engine {
using Price = uint64_t;

/*
 * Fills the order if possible, creates a buy order otherwise.
 * @param sell_book is the active list of sell orders
 * @param buy_book is the active list of buy orders
 * @param order is the order to be traded or added to a book
 */
namespace {
void match_buy(std::map<Price, Inventory, std::less<Price>>& sell_book,
               std::map<Price, Inventory, std::greater<Price>>& buy_book,
               Order& order) {
  // If there's a matching price, sell
  while (!sell_book.empty() && sell_book.begin()->first <= order.price &&
         order.size != 0) {
    auto best_price_it = sell_book.begin();
    Inventory& inv = best_price_it->second;

    while (!inv.order_queue.empty() && order.size > 0) {
      Order& next_to_sell = inv.order_queue.front();

      if (order.size >= next_to_sell.size) {
        order.size -= next_to_sell.size;
        inv.quantity -= next_to_sell.size;
        inv.order_queue.pop_front();
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
  }
}

/*
 * Fills the order if possible, creates a sell order otherwise.
 * @param sell_book is the active list of sell orders
 * @param buy_book is the active list of buy orders
 * @param order is the order to be traded or added to a book
 */
void match_sell(std::map<Price, Inventory, std::less<Price>>& sell_book,
                std::map<Price, Inventory, std::greater<Price>>& buy_book,
                Order& order) {
  // If there's a matching price, buy
  while (!buy_book.empty() && order.price <= buy_book.begin()->first &&
         order.size != 0) {
    auto best_price_it = buy_book.begin();
    Inventory& inv = best_price_it->second;

    while (!inv.order_queue.empty() && order.size > 0) {
      Order& next_to_buy = inv.order_queue.front();

      if (order.size >= next_to_buy.size) {
        order.size -= next_to_buy.size;
        inv.quantity -= next_to_buy.size;
        inv.order_queue.pop_front();
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
    // If there's no match, creates an order on the sell_book
    if (order.size > 0) {
      auto& inv = sell_book[order.price];
      inv.quantity += order.size;
      inv.order_queue.push_back(order);
    }
  }
}
}  // namespace

void match_order(std::map<Price, Inventory, std::less<Price>>& sell_book,
                 std::map<Price, Inventory, std::greater<Price>>& buy_book,
                 Order& order) {
  if (order.action == "C") {
    if (order.buy) {
      Matching_Engine::cancel(buy_book, order);
    } else {
      Matching_Engine::cancel(sell_book, order);
    }
  } else if (order.action == "A") {
    if (order.buy) {
      match_buy(sell_book, buy_book, order);
    } else {
      match_sell(sell_book, buy_book, order);
    }
  } else {
    std::cerr << "Error, order action %s not recognized." + order.action
              << std::endl;
    exit(EXIT_FAILURE);
  }
}
}  // namespace Matching_Engine
