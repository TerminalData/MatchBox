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

}  // namespace Matching_Engine
