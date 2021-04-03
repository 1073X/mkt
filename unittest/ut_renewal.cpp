#include <gtest/gtest.h>

#include <stub/ref.hpp>

#include "mkt/renewal.hpp"
#include "source/lib/mkt/place.hpp"

struct ut_renewal : public testing::Test {
    miu::ref::stub stub;

    char buf[4096 * 2] {};
    miu::mkt::place* place = miu::mkt::place::make(buf, sizeof(buf), stub.db(), 8, 4, "name");
};

TEST_F(ut_renewal, create) {
    auto quotes  = place->get_quotes(2);
    auto depths  = place->get_depths();
    auto renewal = miu::mkt::renewal { quotes, depths };
    EXPECT_TRUE(renewal);

    EXPECT_FALSE(miu::mkt::renewal {});
}

TEST_F(ut_renewal, quote) {
    auto quotes = place->get_quotes(1);
    quotes->inc_index();

    auto renewal = miu::mkt::renewal { quotes, place->get_depths() };
    EXPECT_EQ(quotes->at(1), renewal.quote());
}

TEST_F(ut_renewal, depth) {
    auto depths  = place->get_depths();
    auto renewal = miu::mkt::renewal { place->get_quotes(2), depths };

    EXPECT_EQ(depths->at(1), renewal.depth());
    EXPECT_EQ(depths->at(1), renewal.depth());    // should return the same
}

TEST_F(ut_renewal, increase_index) {
    auto quotes = place->get_quotes(1);
    EXPECT_EQ(0U, quotes->index());
    miu::mkt::renewal { quotes, place->get_depths() };    // deconstructor
    EXPECT_EQ(1U, quotes->index());
    miu::mkt::renewal { quotes, place->get_depths() };    // deconstructor
    EXPECT_EQ(2U, quotes->index());
}
