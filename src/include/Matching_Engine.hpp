#pragma once

#include <cstdio>
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
 * Finds the correct order in the buy book and cancels the right amount from it.
 * @param buy_book is the active list of buy orders
 * @param order is the order to be traded or added to a book
 */
void cancel_buy(std::map<Price, Inventory, std::greater<Price>>& buy_book,
                Order& order);

/*
 * Finds the correct order in the sell book and cancels the right amount from
 * it.
 * @param sell_book is the active list of sell orders
 * @param order is the order to be traded or added to a book
 */
void cancel_sell(std::map<Price, Inventory, std::less<Price>>& sell_book,
                 Order& order);

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
