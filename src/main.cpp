#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <queue>

#include "Inventory.hpp"
#include "Matching_Engine.hpp"
#include "Parser.hpp"

void print_result(size_t order_count,
                  std::chrono::duration<double, std::milli> final_time) {
  std::cout << order_count << " orders in " << final_time.count() << " ms.\n"
            << std::endl;
}

int main() {
  using Price = uint64_t;

  // Benchmark of the whole system
  std::map<Price, Inventory, std::less<Price>> sell_book;
  std::map<Price, Inventory, std::greater<Price>> buy_book;
  std::ifstream file = open_file();
  std::string line;
  size_t order_count = 0;

  auto start_time = std::chrono::high_resolution_clock::now();
  while (std::getline(file, line)) {
    if (line.empty()) continue;
    std::optional<Order> order = parse_json(line);
    if (order) {
      order_count++;
      Matching_Engine::match_order(sell_book, buy_book, order.value());
    }
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto final_time = end_time - start_time;

  std::cout << "Parse and match ";
  print_result(order_count, final_time);

  // Benchmark of parsing
  file.clear();
  file.seekg(0, std::ios::beg);

  std::map<Price, Inventory, std::less<Price>> sell_book2;
  std::map<Price, Inventory, std::greater<Price>> buy_book2;
  order_count = 0;
  std::queue<Order> order_queue;

  start_time = std::chrono::high_resolution_clock::now();
  while (std::getline(file, line)) {
    if (line.empty()) continue;
    std::optional<Order> order = parse_json(line);
    if (order) {
      order_count++;
      order_queue.push(std::move(order.value()));
    }
  }
  end_time = std::chrono::high_resolution_clock::now();
  final_time = end_time - start_time;

  std::cout << "Parse ";
  print_result(order_count, final_time);
  file.close();

  // Benchmark of matching
  start_time = std::chrono::high_resolution_clock::now();
  while (!order_queue.empty()) {
    Matching_Engine::match_order(sell_book2, buy_book2, order_queue.front());
    order_queue.pop();
  }
  end_time = std::chrono::high_resolution_clock::now();
  final_time = end_time - start_time;

  std::cout << "Match ";
  print_result(order_count, final_time);

  return 0;
}
