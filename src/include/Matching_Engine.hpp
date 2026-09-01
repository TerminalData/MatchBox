#pragma once

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <map>

#include "Inventory.hpp"
#include "Order.hpp"

namespace Matching_Engine {
using Price = int;

/*
 * Manages the orders and redistributes them according to their
 * action and side.
 *
 * @param sell_book is the active list of sell orders
 * @param buy_book is the active list of buy orders
 * @param order is the order to be traded or added to a book
 */
void match_order(std::map<Price, Inventory, std::less<Price>>& sell_book,
                 std::map<Price, Inventory, std::greater<Price>>& buy_book,
                 Order& order);

/*
 * Finds the correct order in the buy or sell book and cancels the right amount
 * from it. If the amount removed is bigger than available, cancel entire order.
 * Iterates through Inventory's list in O(n), accepted since that list
 * should never be big enough to justify  O(1) implementation.
 * @param book is the active list of buy orders
 * @param order is the order to be traded or added to a book
 */
template <typename Compare>
void cancel(std::map<Price, Inventory, Compare>& book, Order& cancel_req) {
  auto price_it = book.find(cancel_req.price);
  if (price_it == book.end()) {
    std::cout << "Book transaction impossible, no match found for price "
              << cancel_req.price << std::endl;
    return;
  }

  Inventory& concerned_inv = price_it->second;

  auto it = std::find_if(concerned_inv.order_queue.begin(),
                         concerned_inv.order_queue.end(),
                         [&cancel_req](const Order& o) {
                           return o.order_id == cancel_req.order_id;
                         });

  if (it != concerned_inv.order_queue.end()) {
    if (cancel_req.size >= it->size) {
      concerned_inv.quantity -= it->size;
      concerned_inv.order_queue.erase(it);
    } else {
      it->size -= cancel_req.size;
      concerned_inv.quantity -= cancel_req.size;
    }
  } else {
    std::cout << "Book transaction impossible, no match found for order_id "
              << cancel_req.order_id << std::endl;
  }
}

/*
 * Fills the order if possible, creates a buy order otherwise.
 * @param sell_book is the active list of sell orders
 * @param buy_book is the active list of buy orders
 * @param order is the order to be traded or added to a book
 */
void match_buy(std::map<Price, Inventory, std::less<Price>>& sell_book,
               std::map<Price, Inventory, std::greater<Price>>& buy_book,
               Order& order);

/*
 * Fills the order if possible, creates a sell order otherwise.
 * @param sell_book is the active list of sell orders
 * @param buy_book is the active list of buy orders
 * @param order is the order to be traded or added to a book
 */
void match_sell(std::map<Price, Inventory, std::less<Price>>& sell_book,
                std::map<Price, Inventory, std::greater<Price>>& buy_book,
                Order& order);

}  // namespace Matching_Engine
