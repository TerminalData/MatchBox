#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>

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
  double total_orders = 2075526.0;
  double total_cancel = 0.0;

  std::cout << "#################### MatchBox ###########################"
            << std::endl;

  // **************************************************************************
  // Benchmark of the whole system

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

  Matching_Engine engine;

  for (auto doc : stream) {
    std::optional<Order> order = parse_json(doc.value());
    if (order) {
      if (order->action == Order_action::Cancel) {
        total_cancel++;
      }
      order_count++;
      engine.match_order(order.value());
    }
  }
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> final_time = end_time - start_time;

  double throughput =
      ((total_orders * 1000.0) / final_time.count()) / 1000000.0;

  double cancel_ratio = (total_cancel / total_orders) * 100.0;

  std::cout << "Canceled order ratio : " << cancel_ratio << "%" << std::endl;
  std::cout << "Parse and match ";
  print_result(order_count, final_time, throughput);

  // **************************************************************************
  // Benchmark of parsing

  simdjson::ondemand::parser parser2;
  auto stream2 = parser2.iterate_many(raw_file_data).value();

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

  Matching_Engine engine2;
  while (!order_queue.empty()) {
    engine2.match_order(order_queue.front());
    order_queue.pop();
  }
  end_time = std::chrono::high_resolution_clock::now();
  final_time = end_time - start_time;

  throughput = ((total_orders * 1000.0) / final_time.count()) / 1000000.0;

  std::cout << "Match ";
  print_result(order_count, final_time, throughput);

  return 0;
}
