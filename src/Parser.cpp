
#include "Parser.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using json = nlohmann::json;

std::ifstream open_file() {
  std::cout << "#################### MatchBox ###########################"
            << std::endl;
  std::cout << "Input json file name to be used. Must be located in test/data/ "
            << std::endl;
  std::string file_name;
  std::cin >> file_name;

  // Open file stream
  std::ifstream file("test/data/" + file_name);
  if (!file.is_open()) {
    std::cerr << "Error: could not open this JSON file, does it really exist?"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  return file;
}

std::optional<Order> parse_json(const std::string& line) {
  try {
    // Parse current line
    json order = json::parse(line);

    // Extract values
    if (order["action"] == "T" || order["action"] == "F" ||
        order["action"] == "R")
      return std::nullopt;
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
    return input_order;

  } catch (const json::exception& e) {
    std::cerr << "JSON error on line " << e.what() << "\n";
  }
  return std::nullopt;
}
