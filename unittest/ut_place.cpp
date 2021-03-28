#include <gtest/gtest.h>

#include <stub/ref.hpp>

#include "source/lib/place.hpp"

using miu::mkt::depth_ring;
using miu::mkt::quote_ring;

struct ut_place : public testing::Test {
    miu::ref::stub stub;

    char buf[4096] {};
    miu::mkt::place* place = miu::mkt::place::make(buf, sizeof(buf), stub.db(), 8, 4, "name");
};

TEST_F(ut_place, resolve_size) {
    auto exp = sizeof(miu::mkt::place)
             // quotes
             + 16 * quote_ring::resolve_size(512)
             // depths
             + depth_ring::resolve_size(16);
    EXPECT_EQ(exp, miu::mkt::place::resolve_size(16, 512, 16));
}

TEST_F(ut_place, make) {
    ASSERT_NE(nullptr, place);

    // not enough space
    EXPECT_EQ(nullptr, miu::mkt::place::make(buf, 0, stub.db(), 512, 16, "name"));
}

TEST_F(ut_place, open) {
    auto place = miu::mkt::place::open(buf);
    ASSERT_NE(nullptr, place);
    EXPECT_EQ("name", place->name());
    EXPECT_EQ(stub.db()->name(), place->db_name());
    EXPECT_EQ(stub.db()->signature(), place->db_sign());
}

TEST_F(ut_place, get_quote_ring) {
    auto ring = place->get_quote_ring(0);
    EXPECT_EQ(8U, ring->capacity());
    EXPECT_EQ((quote_ring*)(place + 1), ring);

    EXPECT_EQ((quote_ring*)(place + 10), place->get_quote_ring(1));

    // overflow
    EXPECT_EQ(nullptr, place->get_quote_ring(16));
}

TEST_F(ut_place, get_depth) {
    auto ring = (depth_ring*)(place + 1 + stub.db()->size() * 9);
    EXPECT_EQ(4U, ring->capacity());
    EXPECT_EQ(ring->at(0), place->get_depth(0));
    EXPECT_EQ(ring->at(1), place->get_depth(1));
    EXPECT_EQ(ring->at(4), place->get_depth(4));
    EXPECT_EQ(ring->at(6), place->get_depth(6));
}
