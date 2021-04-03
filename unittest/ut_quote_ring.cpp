#include <gtest/gtest.h>

#include "source/lib/mkt/quote_ring.hpp"

using miu::mkt::quote_ring;

TEST(ut_quote_ring, resolve_size) {
    auto exp = sizeof(quote_ring) + 128 * sizeof(miu::mkt::quote);
    EXPECT_EQ(exp, quote_ring::resolve_size(128));
}

TEST(ut_quote_ring, default) {
    char buf[4096] {};
    auto ring = quote_ring::make(buf, sizeof(buf), 8);
    ASSERT_NE(nullptr, ring);
    EXPECT_EQ(8U, ring->capacity());
    EXPECT_EQ(0U, ring->index());
    EXPECT_EQ(miu::ref::symbol {}, ring->symbol());
    EXPECT_EQ(0U, ring->id());
    EXPECT_EQ(miu::time::stamp {}, ring->time());
    EXPECT_EQ(0U, ring->is_observed());
}

TEST(ut_quote_ring, set_value) {
    char buf[4096] {};
    auto ring = quote_ring::make(buf, sizeof(buf), 8);

    auto time = miu::time::clock::now();
    ring->set_time(time);
    EXPECT_EQ(time, ring->time());

    auto symbol = miu::ref::symbol { "SSE/STOCK/stkname" };
    ring->set_symbol(symbol);
    EXPECT_EQ(symbol, ring->symbol());

    ring->set_id(12);
    EXPECT_EQ(12U, ring->id());

    ring->observe();
    EXPECT_EQ(1U, ring->is_observed());

    EXPECT_EQ(0U, ring->is_subscribed());
    ring->subscribe();
    EXPECT_EQ(1U, ring->is_subscribed());
}

TEST(ut_quote_ring, get) {
    char buf[4096] {};
    auto ring = quote_ring::make(buf, sizeof(buf), 8);

    EXPECT_EQ((miu::mkt::quote const*)(ring + 1), ring->at(0));
    EXPECT_EQ((miu::mkt::quote const*)(ring + 2), ring->at(1));

    // wrapping
    EXPECT_EQ((miu::mkt::quote const*)(ring + 1), ring->at(8));
    EXPECT_EQ((miu::mkt::quote const*)(ring + 3), ring->at(10));
}

TEST(ut_quote_ring, index) {
    char buf[4096] {};
    {
        auto ring = quote_ring::make(buf, sizeof(buf), 8);
        EXPECT_EQ(0U, ring->index());
        ring->inc_index();
        EXPECT_EQ(1U, ring->index());
    }

    // keep index
    auto ring = quote_ring::make(buf, sizeof(buf), 8);
    EXPECT_EQ(1U, ring->index());
}
