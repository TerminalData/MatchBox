
#include "Parser.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

#include "simdjson.h"

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

std::optional<Order> parse_json(const std::string& line,
                                simdjson::ondemand::parser& parser) {
  try {
    simdjson::padded_string padded_line(line);
    simdjson::ondemand::document order = parser.iterate(padded_line);

    // Skip useless orders form input file
    std::string_view action = order["action"];
    if (action == "T" || action == "F" || action == "R") return std::nullopt;

    // Extract values
    simdjson::ondemand::value price_val = order["price"];
    uint64_t price;
    if (price_val.type() == simdjson::ondemand::json_type::string) {
      std::string_view str_val = price_val.get_string();
      price = std::stoull(std::string(str_val));
    } else {
      price = price_val.get_uint64();
    }

    simdjson::ondemand::value size_val = order["size"];
    int size;
    if (size_val.type() == simdjson::ondemand::json_type::string) {
      std::string_view str_val = size_val.get_string();
      size = std::stoi(std::string(str_val));
    } else {
      size = size_val.get_int64();
    }

    simdjson::ondemand::value id_val = order["order_id"];
    int order_id;
    if (id_val.type() == simdjson::ondemand::json_type::string) {
      std::string_view str_val = id_val.get_string();
      order_id = std::stoi(std::string(str_val));
    } else {
      order_id = id_val.get_int64();
    }

    bool buy = order["side"] == "B" ? true : false;

    return Order{price, std::string(action), size, order_id, buy};

  } catch (const simdjson::simdjson_error& e) {
    std::cerr << "JSON error on line " << e.what() << "\n";
    return std::nullopt;
  }
}
