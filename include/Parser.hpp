#pragma once

#include <cstdint>
#include <cstdio>
#include <nlohmann/json.hpp>
#include <optional>

#include "Order.hpp"
#include "simdjson.h"

using Price = uint64_t;
/*
 * Asks the user what json file is to be used as a benchmark test.
 *
 * @return the opened stream file object
 */
std::ifstream open_file();

/*
 * Parses each line fed to it into an Order object.
 *
 * @param line is the current line read of the json file for the benchmark
 * @return either nothing if the order is one not considered by the match
 * engine, or the order object.
 */
std::optional<Order> parse_json(const std::string& line,
                                simdjson::ondemand::parser& parser);
