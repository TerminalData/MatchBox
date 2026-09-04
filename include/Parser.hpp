#pragma once

#include <cstdint>
#include <cstdio>
#include <nlohmann/json.hpp>
#include <optional>

#include "Order.hpp"

using Price = uint64_t;

std::ifstream open_file();
std::optional<Order> parse_json(const std::string& line);
