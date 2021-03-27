#include <gtest/gtest.h>

#include "mkt/quote.hpp"

TEST(ut_quote, default) {
    miu::mkt::quote quote;
    EXPECT_EQ(miu::time::stamp {}, quote.time());
    EXPECT_EQ(0.0, quote.bid());
    EXPECT_EQ(0, quote.bid_vol());
    EXPECT_EQ(0.0, quote.ask());
    EXPECT_EQ(0, quote.ask_vol());
    EXPECT_EQ(0.0, quote.last());
    EXPECT_EQ(0, quote.last_vol());
    EXPECT_EQ(0.0, quote.turnover());
    EXPECT_EQ(0, quote.total_vol());
    EXPECT_EQ(0, quote.open_interest());
    EXPECT_EQ(0, quote.depth_id());
}

TEST(ut_quote, set_value) {
    miu::mkt::quote quote;

    auto time = miu::time::clock::now();
    quote.set_time(time);
    EXPECT_EQ(time, quote.time());

    quote.set_bid(1.2);
    EXPECT_EQ(1.2, quote.bid());

    quote.set_bid_vol(100);
    EXPECT_EQ(100, quote.bid_vol());

    quote.set_ask(2.1);
    EXPECT_EQ(2.1, quote.ask());

    quote.set_ask_vol(200);
    EXPECT_EQ(200, quote.ask_vol());

    quote.set_last(1.3);
    EXPECT_EQ(1.3, quote.last());

    quote.set_last_vol(20);
    EXPECT_EQ(20, quote.last_vol());

    quote.set_turnover(11223344);
    EXPECT_EQ(11223344, quote.turnover());

    quote.set_total_vol(12345);
    EXPECT_EQ(12345, quote.total_vol());

    quote.set_open_interest(4321);
    EXPECT_EQ(4321, quote.open_interest());

    quote.set_depth_id(3);
    EXPECT_EQ(3U, quote.depth_id());
}
