
#include "Parser.hpp"

#include <charconv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string_view>

#include "Order.hpp"
#include "simdjson.h"

using json = nlohmann::json;

std::optional<Order> parse_json(simdjson::ondemand::document_reference doc) {
  try {
    // Skip useless orders form input file
    if (doc["action"] != "A" && doc["action"] != "C") return std::nullopt;

    // Extract values
    Order_action action;
    doc["action"] == "A" ? action = Order_action::Add
                         : action = Order_action::Cancel;

    simdjson::ondemand::value price_val = doc["price"];
    uint64_t price = 0;
    if (price_val.type() == simdjson::ondemand::json_type::string) {
      std::string_view str_val = price_val.get_string();
      std::from_chars(str_val.data(), str_val.data() + str_val.size(), price);
    } else {
      price = static_cast<uint64_t>(price_val.get_uint64());
    }

    simdjson::ondemand::value size_val = doc["size"];
    uint32_t size = 0;
    if (size_val.type() == simdjson::ondemand::json_type::string) {
      std::string_view str_val = size_val.get_string();
      std::from_chars(str_val.data(), str_val.data() + str_val.size(), size);
    } else {
      size = static_cast<uint32_t>(size_val.get_uint32());
    }

    simdjson::ondemand::value id_val = doc["order_id"];
    uint32_t order_id = 0;
    if (id_val.type() == simdjson::ondemand::json_type::string) {
      std::string_view str_val = id_val.get_string();
      std::from_chars(str_val.data(), str_val.data() + str_val.size(),
                      order_id);
    } else {
      order_id = static_cast<uint32_t>(id_val.get_uint32());
    }

    bool buy = doc["side"] == "B" ? true : false;
    if (order_id == 0 || size == 0 || price == 0) {
      return std::nullopt;
    }

    return Order{price, order_id, size, action, buy};

  } catch (const simdjson::simdjson_error &e) {
    std::cerr << "JSON error on line " << e.what() << "\n";
    return std::nullopt;
  }
}
