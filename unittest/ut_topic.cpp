#include <gtest/gtest.h>

#include <stub/ref.hpp>

#include "mkt/topic.hpp"
#include "source/lib/mkt/place.hpp"

struct ut_topic : public testing::Test {
    miu::ref::stub stub;

    char buf[4096 * 2] {};
    miu::mkt::place* place = miu::mkt::place::make(buf, sizeof(buf), stub.db(), 8, 4, "name");
};

TEST_F(ut_topic, create) {
    auto quotes = place->get_quotes(1);
    auto depths = place->get_depths();
    miu::mkt::topic topic { quotes, depths };

    EXPECT_EQ(1U, topic.id());
    EXPECT_EQ(stub.db()->find(1).symbol(), topic.symbol());
    EXPECT_EQ(0.0, topic.bid());
    EXPECT_EQ(0, topic.bid_vol());
    EXPECT_EQ(0.0, topic.ask());
    EXPECT_EQ(0, topic.ask_vol());
    EXPECT_EQ(0.0, topic.last());
    EXPECT_EQ(0, topic.last_vol());
    EXPECT_EQ(0.0, topic.turnover());
    EXPECT_EQ(0, topic.total_vol());
    EXPECT_EQ(0, topic.open_interest());

    EXPECT_FALSE(topic.is_subscribed());
}

TEST_F(ut_topic, refresh) {
    auto quotes = place->get_quotes(1);
    quotes->at(0)->set_last(1);
    quotes->inc_index();

    auto depths = place->get_depths();
    miu::mkt::topic topic { quotes, depths };

    EXPECT_EQ(1.0, topic.last());

    EXPECT_EQ(0U, topic.refresh());

    quotes->at(1)->set_last(2);
    quotes->inc_index();
    EXPECT_EQ(1U, topic.refresh());
    EXPECT_EQ(2.0, topic.last());

    quotes->subscribe();
    EXPECT_TRUE(topic.is_subscribed());
}

TEST_F(ut_topic, depth) {
    auto quotes = place->get_quotes(1);
    quotes->at(0)->set_depth_id(5);
    quotes->inc_index();

    auto depths = place->get_depths();
    depths->at(5)->set_bid(1, 1.2);
    depths->at(5)->set_ask(2, 2.1);

    miu::mkt::topic topic { quotes, depths };
    EXPECT_EQ(1.2, topic.bid(1));
    EXPECT_EQ(0, topic.bid_vol(1));
    EXPECT_EQ(2.1, topic.ask(2));
    EXPECT_EQ(0, topic.ask_vol(1));
}
