#include <gtest/gtest.h>

#include "Matching_Engine.hpp"
#include "Order.hpp"

class MatchingEngineTest : public ::testing::Test
{
protected:
    Matching_Engine engine;
};

TEST_F(MatchingEngineTest, KeepsNonCrossingOrdersAvailable)
{
    Order buy{100, 1, 10, Order_action::Add, true};
    Order sell{105, 2, 10, Order_action::Add, false};
    engine.match_order(buy);
    engine.match_order(sell);

    Order take_buy{100, 3, 10, Order_action::Add, false};
    Order take_sell{105, 4, 10, Order_action::Add, true};
    engine.match_order(take_buy);
    engine.match_order(take_sell);

    EXPECT_EQ(take_buy.size, 0);
    EXPECT_EQ(take_sell.size, 0);
}

TEST_F(MatchingEngineTest, HandlesPartialFills)
{
    Order sell{100, 1, 10, Order_action::Add, false};
    Order buy{100, 2, 4, Order_action::Add, true};
    engine.match_order(sell);
    engine.match_order(buy);

    EXPECT_EQ(buy.size, 0);

    Order second_buy{100, 3, 10, Order_action::Add, true};
    engine.match_order(second_buy);
    EXPECT_EQ(second_buy.size, 4);
}

TEST_F(MatchingEngineTest, FullyConsumedOrdersLeaveNoAvailableQuantity)
{
    Order sell{100, 1, 5, Order_action::Add, false};
    Order buy{100, 2, 5, Order_action::Add, true};
    engine.match_order(sell);
    engine.match_order(buy);

    Order follow_up{100, 3, 1, Order_action::Add, true};
    engine.match_order(follow_up);
    EXPECT_EQ(follow_up.size, 1);
}

TEST_F(MatchingEngineTest, RespectsFifoOrderPriority)
{
    Order first{100, 101, 5, Order_action::Add, false};
    Order second{100, 102, 5, Order_action::Add, false};
    engine.match_order(first);
    engine.match_order(second);

    Order partial_buy{100, 201, 7, Order_action::Add, true};
    engine.match_order(partial_buy);

    Order cancel_second{100, 102, 3, Order_action::Cancel, false};
    engine.match_order(cancel_second);
    Order final_buy{100, 202, 3, Order_action::Add, true};
    engine.match_order(final_buy);

    EXPECT_EQ(final_buy.size, 3);
}

TEST_F(MatchingEngineTest, SweepsMultiplePriceLevelsAndRestsRemainder)
{
    Order first_sell{100, 1, 5, Order_action::Add, false};
    Order second_sell{101, 2, 5, Order_action::Add, false};
    Order buy{102, 3, 15, Order_action::Add, true};
    engine.match_order(first_sell);
    engine.match_order(second_sell);
    engine.match_order(buy);

    EXPECT_EQ(buy.size, 5);

    Order take_remainder{102, 4, 5, Order_action::Add, false};
    engine.match_order(take_remainder);
    EXPECT_EQ(take_remainder.size, 0);
}

TEST_F(MatchingEngineTest, FullCancellationPreventsMatching)
{
    Order buy{100, 1, 10, Order_action::Add, true};
    engine.match_order(buy);

    Order cancel{100, 1, 10, Order_action::Cancel, true};
    engine.match_order(cancel);

    Order sell{100, 2, 10, Order_action::Add, false};
    engine.match_order(sell);
    EXPECT_EQ(sell.size, 10);
}

TEST_F(MatchingEngineTest, PartialCancellationLeavesTheRemainingQuantity)
{
    Order sell{100, 1, 10, Order_action::Add, false};
    engine.match_order(sell);

    Order cancel{100, 1, 4, Order_action::Cancel, false};
    engine.match_order(cancel);

    Order buy{100, 2, 10, Order_action::Add, true};
    engine.match_order(buy);
    EXPECT_EQ(buy.size, 4);
}

TEST_F(MatchingEngineTest, OverCancellationRemovesTheAvailableQuantity)
{
    Order sell{100, 1, 5, Order_action::Add, false};
    engine.match_order(sell);

    Order cancel{100, 1, 20, Order_action::Cancel, false};
    engine.match_order(cancel);

    Order buy{100, 2, 1, Order_action::Add, true};
    engine.match_order(buy);
    EXPECT_EQ(buy.size, 1);
}

TEST_F(MatchingEngineTest, MissingCancellationDoesNotCorruptTheEngine)
{
    Order cancel{100, 999, 10, Order_action::Cancel, true};
    engine.match_order(cancel);

    Order sell{100, 1, 5, Order_action::Add, false};
    engine.match_order(sell);
    Order buy{100, 2, 5, Order_action::Add, true};
    engine.match_order(buy);

    EXPECT_EQ(buy.size, 0);
}
