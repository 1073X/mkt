#include <gtest/gtest.h>

#include <log/log.hpp>
#include <shm/tempfs.hpp>
#include <stub/ref.hpp>

#include "mkt/reg_var_str.hpp"
#include "mkt/source.hpp"
#include "source/lib/mkt/place.hpp"

struct ut_source : public testing::Test {
    void SetUp() override {
        miu::mkt::reg_var_str();
        // miu::log::reset(miu::log::severity::DEBUG, 1024);

        miu::shm::buffer buf { { stub.marker(), "mkt" }, 8192 };
        miu::mkt::place::make(buf.data(), buf.size(), stub.db(), 8, 4, stub.marker());
    }

    void TearDown() override {
        miu::shm::tempfs::remove(stub.marker(), "mkt");
        miu::log::dump();
    }

    miu::ref::stub stub;
};

TEST_F(ut_source, create) {
    miu::mkt::source source { stub.marker() };
    EXPECT_TRUE(source);
    EXPECT_EQ(stub.marker(), source.db_name());
}

TEST_F(ut_source, empty) {
    auto source = miu::mkt::source {};
    EXPECT_FALSE(source);
    EXPECT_FALSE(source.subscribe(0));
    EXPECT_FALSE(source.subscribe(miu::ref::symbol("SSE/BOND/ABC")));

    EXPECT_FALSE(miu::mkt::source { "not_exists" });
}

TEST_F(ut_source, subscribe_by_instrument_id) {
    miu::mkt::source source { stub.marker() };

    auto topic = source.subscribe(1);
    ASSERT_TRUE(topic);
    EXPECT_EQ(1, topic.id());

    EXPECT_FALSE(source.subscribe(stub.db()->num_of_instrument()));
}

TEST_F(ut_source, observer) {
    miu::mkt::source { stub.marker() }.subscribe(1);

    miu::shm::buffer buf { { stub.marker(), "mkt" }, 8192 };
    auto place  = miu::mkt::place::open(buf.data());
    auto quotes = place->get_quotes(1);
    EXPECT_EQ(1U, quotes->is_observed());
}

TEST_F(ut_source, subscribe_by_symbol) {
    miu::mkt::source source { stub.marker() };

    auto inst  = stub.db()->find(3);
    auto topic = source.subscribe(inst.symbol());
    ASSERT_TRUE(topic);
    EXPECT_EQ(3U, topic.id());

    EXPECT_FALSE(source.subscribe(miu::ref::symbol("SSE/BOND/ABC")));
}
