#include <gtest/gtest.h>

#include <optional>
#include <string_view>

#include "Parser.hpp"
#include "simdjson.h"

namespace
{
    std::optional<Order> parse_record(std::string_view record)
    {
        simdjson::ondemand::parser parser;
        simdjson::padded_string padded(record);
        auto document = parser.iterate(padded);
        if (document.error())
            return std::nullopt;
        return parse_json(document.value());
    }
} // namespace

TEST(ParserTest, ParsesNumericStringsAndNumericValues)
{
    const auto string_order = parse_record(
        R"({"action":"A","price":"100","size":"12","order_id":"42","side":"B"})");
    ASSERT_TRUE(string_order.has_value());
    EXPECT_EQ(string_order->price, 100);
    EXPECT_EQ(string_order->size, 12);
    EXPECT_EQ(string_order->order_id, 42);
    EXPECT_EQ(string_order->action, Order_action::Add);
    EXPECT_TRUE(string_order->buy);

    const auto numeric_order = parse_record(
        R"({"action":"C","price":101,"size":4,"order_id":43,"side":"S"})");
    ASSERT_TRUE(numeric_order.has_value());
    EXPECT_EQ(numeric_order->price, 101);
    EXPECT_EQ(numeric_order->size, 4);
    EXPECT_EQ(numeric_order->order_id, 43);
    EXPECT_EQ(numeric_order->action, Order_action::Cancel);
    EXPECT_FALSE(numeric_order->buy);
}

TEST(ParserTest, IgnoresUnsupportedActions)
{
    EXPECT_FALSE(parse_record(
                     R"({"action":"T","price":"100","size":"1","order_id":"1","side":"B"})")
                     .has_value());
}

TEST(ParserTest, RejectsMalformedNumericStrings)
{
    EXPECT_FALSE(parse_record(
                     R"({"action":"A","price":"100oops","size":"1","order_id":"1","side":"B"})")
                     .has_value());
    EXPECT_FALSE(parse_record(
                     R"({"action":"A","price":"100","size":"1x","order_id":"1","side":"B"})")
                     .has_value());
}

TEST(ParserTest, RejectsMissingAndZeroRequiredValues)
{
    EXPECT_FALSE(parse_record(
                     R"({"action":"A","price":"100","size":"1","side":"B"})")
                     .has_value());
    EXPECT_FALSE(parse_record(
                     R"({"action":"A","price":"0","size":"1","order_id":"1","side":"B"})")
                     .has_value());
    EXPECT_FALSE(parse_record(
                     R"({"action":"A","price":"100","size":"0","order_id":"1","side":"B"})")
                     .has_value());
}

TEST(ParserTest, RejectsMalformedJson)
{
    EXPECT_FALSE(parse_record(
                     R"({"action":"A","price":"100","size":"1")")
                     .has_value());
}