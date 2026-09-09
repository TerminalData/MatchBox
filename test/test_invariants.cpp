#include <gtest/gtest.h>

#include "Matching_Engine.hpp"
#include "Order.hpp"

TEST(InvariantTest, MatchingNeverCreatesQuantityFromNothing)
{
    Matching_Engine engine;

    Order sell{100, 1, 10, Order_action::Add, false};
    engine.match_order(sell);

    Order buy{100, 2, 6, Order_action::Add, true};
    engine.match_order(buy);

    EXPECT_EQ(buy.size, 0);

    Order remaining_buy{100, 3, 5, Order_action::Add, true};
    engine.match_order(remaining_buy);
    EXPECT_EQ(remaining_buy.size, 1);
}

TEST(InvariantTest, CancellationDoesNotExposeCanceledQuantity)
{
    Matching_Engine engine;

    Order buy{100, 1, 10, Order_action::Add, true};
    engine.match_order(buy);

    Order cancel{100, 1, 4, Order_action::Cancel, true};
    engine.match_order(cancel);

    Order sell{100, 2, 10, Order_action::Add, false};
    engine.match_order(sell);
    EXPECT_EQ(sell.size, 4);
}

TEST(InvariantTest, FIFOOrderCanBeObservedThroughCancellation)
{
    Matching_Engine engine;

    Order first{100, 10, 3, Order_action::Add, false};
    Order second{100, 11, 4, Order_action::Add, false};
    engine.match_order(first);
    engine.match_order(second);

    Order buy_first{100, 20, 3, Order_action::Add, true};
    engine.match_order(buy_first);

    Order cancel_second{100, 11, 4, Order_action::Cancel, false};
    engine.match_order(cancel_second);
    Order buy_after_cancel{100, 21, 1, Order_action::Add, true};
    engine.match_order(buy_after_cancel);

    EXPECT_EQ(buy_after_cancel.size, 1);
}
