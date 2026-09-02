#include "../include/Matching_Engine.hpp"

#include <cstdio>
#include <iostream>

namespace Matching_Engine {
using Price = int;

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
  // If there's not match, creates an order on the buy_book
  if (order.size > 0) {
    auto& inv = buy_book[order.price];
    inv.order_queue.push_back(order);
    inv.quantity += order.size;
  }
}

// TODO: implement match_sell based on match_buy

}  // namespace Matching_Engine
