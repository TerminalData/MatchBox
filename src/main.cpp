#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <queue>

#include "Inventory.hpp"
#include "Matching_Engine.hpp"
#include "Parser.hpp"
#include "simdjson.h"

void print_result(size_t order_count,
                  std::chrono::duration<double, std::milli> final_time,
                  double throughput) {
  std::cout << order_count << " orders in " << final_time.count() << " ms."
            << std::endl;
  std::cout << "Throughput MOPS = " << throughput << "\n" << std::endl;
}

int main() {
  using Price = uint64_t;
  double total_orders = 2075526.0;

  std::cout << "#################### MatchBox ###########################"
            << std::endl;

  // **************************************************************************
  // Benchmark of the whole system

  std::map<Price, Inventory, std::less<Price>> sell_book;
  std::map<Price, Inventory, std::greater<Price>> buy_book;

  simdjson::padded_string raw_file_data;
  simdjson::error_code error =
      simdjson::padded_string::load("test/data/test1.json").get(raw_file_data);

  if (error) {
    std::cerr << "ERROR: could not open JSON file." << std::endl;
    exit(EXIT_FAILURE);
  }

  simdjson::ondemand::parser parser;
  auto stream = parser.iterate_many(raw_file_data).value();

  size_t order_count = 0;

  auto start_time = std::chrono::high_resolution_clock::now();
  for (auto doc : stream) {
    std::optional<Order> order = parse_json(doc.value());
    if (order) {
      order_count++;
      Matching_Engine::match_order(sell_book, buy_book, order.value());
    }
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> final_time = end_time - start_time;

  double throughput =
      ((total_orders * 1000.0) / final_time.count()) / 1000000.0;

  std::cout << "Parse and match ";
  print_result(order_count, final_time, throughput);

  // **************************************************************************
  // Benchmark of parsing

  simdjson::ondemand::parser parser2;
  auto stream2 = parser2.iterate_many(raw_file_data).value();

  std::map<Price, Inventory, std::less<Price>> sell_book2;
  std::map<Price, Inventory, std::greater<Price>> buy_book2;
  order_count = 0;
  std::queue<Order> order_queue;

  start_time = std::chrono::high_resolution_clock::now();
  for (auto doc : stream2) {
    std::optional<Order> order = parse_json(doc.value());
    if (order) {
      order_count++;
      order_queue.push(std::move(order.value()));
    }
  }
  end_time = std::chrono::high_resolution_clock::now();

  final_time = end_time - start_time;

  throughput = ((total_orders * 1000.0) / final_time.count()) / 1000000.0;

  std::cout << "Parse ";
  print_result(order_count, final_time, throughput);

  // **************************************************************************
  // Benchmark of matching
  start_time = std::chrono::high_resolution_clock::now();
  while (!order_queue.empty()) {
    Matching_Engine::match_order(sell_book2, buy_book2, order_queue.front());
    order_queue.pop();
  }
  end_time = std::chrono::high_resolution_clock::now();
  final_time = end_time - start_time;

  throughput = ((total_orders * 1000.0) / final_time.count()) / 1000000.0;

  std::cout << "Match ";
  print_result(order_count, final_time, throughput);

  return 0;
}
