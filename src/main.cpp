#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

#include "Inventory.hpp"
#include "Matching_Engine.hpp"

using json = nlohmann::json;

int main() {
  using Price = uint64_t;

  // Create the Sell order book
  std::map<Price, Inventory, std::less<Price>> sell_book;

  // Create the Buy order book
  std::map<Price, Inventory, std::greater<Price>> buy_book;
  std::cout << "#################### MatchBox ###########################"
            << std::endl;
  std::cout << "Input json file name to be used. Must be located in test/data/ "
            << std::endl;
  std::string file_name;
  std::cin >> file_name;

  auto start_time = std::chrono::high_resolution_clock::now();
  // Open file stream
  std::ifstream file("test/data/" + file_name);
  if (!file.is_open()) {
    std::cerr << "Error: could not open this JSON file, does it really exist?"
              << std::endl;
    return 1;
  }

  std::string line;
  size_t order_count = 0;

  // Iterate through the file line by line
  while (std::getline(file, line)) {
    if (line.empty()) continue;

    try {
      // Parse current line
      json order = json::parse(line);
      ++order_count;

      // Extract values
      if (order["action"] == "T" || order["action"] == "F" ||
          order["action"] == "R")
        continue;
      std::string action = order["action"];
      uint64_t price = order["price"].is_string()
                           ? std::stoull(order["price"].get<std::string>())
                           : order["price"].get<uint64_t>();
      int size = order["size"].is_string()
                     ? std::stoi(order["size"].get<std::string>())
                     : order["size"].get<int>();
      int order_id = order["order_id"].is_string()
                         ? std::stoi(order["order_id"].get<std::string>())
                         : order["order_id"].get<int>();
      uint64_t timestamp =
          order["hd"]["ts_event"].is_string()
              ? std::stoull(order["hd"]["ts_event"].get<std::string>())
              : order["hd"]["ts_event"].get<uint64_t>();
      bool buy = order["side"] == "B" ? true : false;

      Order input_order{price, action, size, order_id, timestamp, buy};

      Matching_Engine::match_order(sell_book, buy_book, input_order);

    } catch (const json::exception& e) {
      std::cerr << "JSON error on line " << order_count << ":" << e.what()
                << "\n";
    }
  }
  file.close();

  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

  std::cout << "Engine processed " << order_count << " orders in "
            << elapsed.count() << " ms.\n"
            << std::endl;

  return 0;
}
