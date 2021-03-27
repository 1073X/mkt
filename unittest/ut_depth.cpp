#include <gtest/gtest.h>

#include "mkt/depth.hpp"

TEST(ut_depth, default) {
    EXPECT_EQ(5U, miu::mkt::depth::max_levels());

    miu::mkt::depth depth;

    for (auto i = 0U; i < miu::mkt::depth::max_levels(); i++) {
        EXPECT_EQ(0.0, depth.bid(i));
        EXPECT_EQ(0, depth.bid_vol(i));
        EXPECT_EQ(0.0, depth.ask(i));
        EXPECT_EQ(0, depth.ask_vol(i));
    }
}

TEST(ut_depth, set_value) {
    miu::mkt::depth depth;

    for (auto i = 0U; i < miu::mkt::depth::max_levels(); i++) {
        depth.set_bid(i, i + 5 - i);
        depth.set_bid_vol(i, 100 * (i + 1));
        depth.set_ask(i, i + 5 + i);
        depth.set_ask_vol(i, 200 * (i + 1));
    }

    for (auto i = 0U; i < miu::mkt::depth::max_levels(); i++) {
        EXPECT_EQ(i + 5 - i, depth.bid(i));
        EXPECT_EQ(100 * (i + 1), depth.bid_vol(i));
        EXPECT_EQ(i + 5 + i, depth.ask(i));
        EXPECT_EQ(200 * (i + 1), depth.ask_vol(i));
    }
}
