#include <gtest/gtest.h>

#include <shm/tempfs.hpp>

#include "stub/mkt.hpp"

TEST(ut_stub, database) {
    miu::mkt::stub stub;
    EXPECT_NE(nullptr, stub.db());
}

TEST(ut_stub, create) {
    std::string file;
    {
        miu::mkt::stub stub;
        file = miu::com::strcat { stub.marker(), "mkt" }.str();
        EXPECT_TRUE(miu::shm::tempfs::exists(file));
    }

    // remove shm when stub desconstructing
    EXPECT_FALSE(miu::shm::tempfs::exists(file));
}

TEST(ut_stub, subscribe) {
    miu::mkt::stub stub;
    auto topic = stub.subscribe(1);
    EXPECT_TRUE(topic);

    EXPECT_FALSE(stub.subscribe(stub.db()->num_of_instrument()));
}

TEST(ut_stub, renew_quote) {
    miu::mkt::stub stub;
    auto topic = stub.subscribe(3);

    stub.renew(3, [](auto quote) { quote->set_bid(1.2); });

    EXPECT_EQ(1, topic.refresh());
    EXPECT_EQ(1.2, topic.bid());
}

TEST(ut_stub, renew_depth) {
    miu::mkt::stub stub;
    auto topic = stub.subscribe(2);

    stub.renew(2, [](auto, auto depth) { depth->set_bid(1, 1.2); });

    EXPECT_EQ(1, topic.refresh());
    EXPECT_EQ(1.2, topic.bid(1));
}
