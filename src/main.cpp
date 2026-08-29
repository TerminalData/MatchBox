#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

#include "include/Inventory.hpp"

using json = nlohmann::json;

int main() {
  using Price = int;

  // Create the Sell order book
  std::map<Price, Inventory, std::less<Price>> sell_book;

  // Create the Buy order book
  std::map<Price, Inventory, std::greater<Price>> buy_book;

  std::cout << "Input json file name to be used. Must be located in test/."
            << std::endl;
  std::string file_name;
  std::cin >> file_name;

  // Open file stream
  std::ifstream file("test/" + file_name);
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
      if (order["action"] == "T") continue;
      std::string action = order["action"];
      uint64_t price = order["price"];
      int size = order["size"];
      int order_id = order["order_id"];
      uint64_t timestamp = order["hd"]["ts_event"];
      bool buy = order["side"] == "B" ? true : false;

      // TODO: create prder object here and call funtion from Trader.cpp

    } catch (const json::parse_error& e) {
      std::cerr << "Parse error from input file on line " << order_count + 1
                << ":" << e.what() << "\n";
    }
  }
  file.close();
  return 0;
}
