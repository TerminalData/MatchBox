#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

#include "Execution.hpp"
#include "Inventory.hpp"
#include "Order.hpp"

class Matching_Engine {
 private:
  using Price = uint64_t;
  using Id = uint32_t;
  using Index = uint32_t;
  std::vector<Order> pool;
  uint32_t free_head = 0;

  std::map<Price, Inventory, std::less<Price>> sell_book;
  std::map<Price, Inventory, std::greater<Price>> buy_book;
  std::unordered_map<Id, Index> active_orders;

  template <typename Own_book, typename Other_book, typename Trade_rule>
  void match_against(Order& order, Own_book& own_book, Other_book& other_book,
                     Trade_rule rule) {
    if (active_orders.contains(order.order_id)) {
      std::cerr << "Error, this order " << order.order_id
                << " cannot be added to the book, it already exists within "
                   "the active list.\n"
                << std::endl;
      return;
    }

    // If there's a matching price, sell
    while (!other_book.empty() &&
           rule(other_book.begin()->first, order.price) && order.size != 0) {
      auto best_price_it = other_book.begin();
      Inventory& inv = best_price_it->second;

      while (!inv.is_empty() && order.size > 0) {
        const uint32_t current_index = inv.head;
        Order& next_to_sell = pool[current_index];

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
        other_book.erase(best_price_it);
      }
    }
    // If there's no match, creates an order on the book
    if (order.size > 0) {
      auto& inv = own_book[order.price];
      if (free_head == NULL_INDEX) {
        std::cerr << "Error, order pool exhausted.\n";
        return;
      }

      const uint32_t new_index = free_head;
      Order& stored_order = pool[new_index];
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

  /*
   * Fills the order if possible, creates a buy order otherwise.
   * @param order is the order to be traded or added to a book
   */
  void match_buy(Order& order);

  /*
   * Fills the order if possible, creates a sell order otherwise.
   * @param order is the order to be traded or added to a book
   */
  void match_sell(Order& order);

  /*
   * Finds the correct order in the active_orders book, and switches it
   * is_canceled field to true (tombstoning).
   *
   * @param order is the order to be canceled.
   */
  void cancel_order(Order& cancel_req);

 public:
  std::vector<Execution>* execution_reports = nullptr;
  explicit Matching_Engine(std::size_t pool_size = 2100000,
                           std::vector<Execution>* reports = nullptr)
      : execution_reports(reports) {
    pool.resize(pool_size);

    pool[0].next_index = 1;

    for (uint32_t i = 1; i < pool_size - 1; ++i) {
      pool[i].next_index = i + 1;
      pool[i].prev_index = i - 1;
    }
    pool.back().next_index = NULL_INDEX;
  }

  /*
   * Manages the orders and redistributes them according to their
   * action and side.
   *
   * @param order is the order to be traded or added to a book
   */
  void match_order(Order& order);
};
