#include <gtest/gtest.h>

#include <log/log.hpp>
#include <stub/ref.hpp>

#include "mkt/reg_var_str.hpp"
#include "source/lib/place.hpp"

using miu::mkt::depth_ring;
using miu::mkt::quote_ring;

struct ut_place : public testing::Test {
    void SetUp() override {
        miu::mkt::reg_var_str();
        // miu::log::reset(miu::log::severity::DEBUG, 1024);
    }
    void TearDown() override { miu::log::dump(); }

    miu::ref::stub stub;

    char buf[4096 * 2] {};
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
    EXPECT_ANY_THROW(miu::mkt::place::make(buf, 0, stub.db(), 512, 16, "name"));
    // can't change quote pre line
    EXPECT_ANY_THROW(miu::mkt::place::make(buf, sizeof(buf), stub.db(), 16, 4, "name"));
    EXPECT_ANY_THROW(miu::mkt::place::make(buf, sizeof(buf), stub.db(), 4, 4, "name"));
}

TEST_F(ut_place, open) {
    auto place = miu::mkt::place::open(buf);
    ASSERT_NE(nullptr, place);
    EXPECT_EQ("name", place->name());
    EXPECT_EQ(stub.db()->name(), place->db_name());
    EXPECT_EQ(stub.db()->signature(), place->db_sign());
}

TEST_F(ut_place, get_quotes) {
    auto ring = place->get_quotes(0);
    EXPECT_EQ(8U, ring->capacity());
    EXPECT_EQ((quote_ring*)(place + 1), ring);
    EXPECT_EQ(0U, ring->id());
    EXPECT_EQ(stub.db()->find(0).symbol(), ring->symbol());
    EXPECT_EQ(0U, ring->index());

    EXPECT_EQ((quote_ring*)(place + 10), place->get_quotes(1));

    // overflow
    EXPECT_EQ(nullptr, place->get_quotes(stub.db()->num_of_instrument()));
    EXPECT_EQ(nullptr, place->get_quotes(stub.db()->max_of_instrument()));
}

TEST_F(ut_place, get_depths) {
    auto ring = (depth_ring*)(place + 1 + stub.db()->max_of_instrument() * 9);
    EXPECT_EQ(4U, ring->capacity());
}
