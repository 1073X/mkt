#include <gtest/gtest.h>

#include "source/lib/quote_ring.hpp"

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
    EXPECT_EQ(0U, ring->num_of_obs());
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

    ring->subscribe();
    EXPECT_EQ(1U, ring->num_of_obs());
    ring->subscribe();
    EXPECT_EQ(2U, ring->num_of_obs());
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

TEST(ut_quote_ring, next) {
    char buf[4096] {};
    auto ring = quote_ring::make(buf, sizeof(buf), 8);

    ring->next([](auto quote) { quote->set_last(1); });
    ring->next([](auto quote) { quote->set_last(2); });

    EXPECT_EQ(1, ring->at(0)->last());
    EXPECT_EQ(2, ring->at(1)->last());

    // wrapping
    for (auto i = ring->index(); i < ring->capacity(); i++) {
        ring->next([i](auto quote) { quote->set_last(i); });
    }

    ring->next([](auto quote) { quote->set_last(10); });
    ring->next([](auto quote) { quote->set_last(20); });

    EXPECT_EQ(10, ring->at(0)->last());
    EXPECT_EQ(20, ring->at(1)->last());
}

TEST(ut_quote_ring, index) {
    char buf[4096] {};
    {
        auto ring = quote_ring::make(buf, sizeof(buf), 8);
        ring->next([](auto) {});
        EXPECT_EQ(1U, ring->index());
        ring->next([](auto) {});
        EXPECT_EQ(2U, ring->index());
    }

    // keep index
    auto ring = quote_ring::make(buf, sizeof(buf), 8);
    EXPECT_EQ(2U, ring->index());
}
