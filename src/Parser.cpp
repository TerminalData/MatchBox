
#include "Parser.hpp"

#include <charconv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string_view>

#include "Order.hpp"
#include "simdjson.h"

namespace {
template <typename Integer>
bool parse_integer(std::string_view value, Integer &result) {
  const auto [end, error] =
      std::from_chars(value.data(), value.data() + value.size(), result);
  if (error == std::errc{} && end == value.data() + value.size()) {
    return true;
  } else if (error == std::errc{}) {
    std::cout << "JSON line has an error: trailing or invalid characters in a "
                 "numeric field.\n"
              << std::endl;
  } else {
    std::cout << "JSON error : " << std::make_error_code(error).message()
              << "\n"
              << std::endl;
  }
  return false;
}
}  // namespace

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
      if (!parse_integer(str_val, price)) return std::nullopt;
    } else {
      price = static_cast<uint64_t>(price_val.get_uint64());
    }

    simdjson::ondemand::value size_val = doc["size"];
    uint32_t size = 0;
    if (size_val.type() == simdjson::ondemand::json_type::string) {
      std::string_view str_val = size_val.get_string();
      if (!parse_integer(str_val, size)) return std::nullopt;
    } else {
      size = static_cast<uint32_t>(size_val.get_uint32());
    }

    simdjson::ondemand::value id_val = doc["order_id"];
    uint32_t order_id = 0;
    if (id_val.type() == simdjson::ondemand::json_type::string) {
      std::string_view str_val = id_val.get_string();
      if (!parse_integer(str_val, order_id)) return std::nullopt;
    } else {
      order_id = static_cast<uint32_t>(id_val.get_uint32());
    }

    bool buy = doc["side"] == "B" ? true : false;
    if (order_id == 0 || size == 0 || price == 0) {
      return std::nullopt;
    }

    return Order{price, order_id, size, action, buy};
  } catch (const simdjson::simdjson_error &e) {
    std::cerr << "JSON error " << e.what() << "\n";
    return std::nullopt;
  }
}
