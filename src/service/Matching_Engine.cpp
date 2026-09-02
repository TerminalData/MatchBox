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
  if (!sell_book.empty()) {
    while (!sell_book.empty() && sell_book.begin()->first <= order.price &&
           order.size != 0) {
      auto best_price_it = sell_book.begin();
      Inventory& inv = best_price_it->second;

      while (!inv.order_queue.empty()) {
        order.size -= sell_book.begin()->second.order_queue.front().size;
        sell_book.begin()->second.quantity -=
            sell_book.begin()->second.order_queue.front().size;
        sell_book.begin()->second.order_queue.pop_front();
        if (sell_book.begin()->second.quantity == 0) {
          sell_book.erase(sell_book.begin());
        }
      }
      if (sell_book.begin()->first <= order.price && order.size != 0) {
        sell_book.begin()->second.order_queue.begin()->size -= order.size;
        sell_book.begin()->second.quantity -= order.size;
        order.size = 0;
        return;
      }
    }
    buy_book[order.price].order_queue.push_back(order);
    buy_book[order.price].quantity += order.size;
  }
}

}  // namespace Matching_Engine
