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
      cancel_buy(buy_book, order);
    } else {
      cancel_sell(sell_book, order);
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

void cancel_buy(std::map<Price, Inventory, std::greater<Price>>& buy_book,
                Order& order) {
  Inventory concerned_inv = buy_book[order.price];
  if (concerned_inv.quantity == 1) {
    Order& concerned_order = concerned_inv.order_queue.front();

    // TODO: finnish impl
  }
}

}  // namespace Matching_Engine
