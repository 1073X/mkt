#include <gtest/gtest.h>

#include "source/lib/mkt/depth_ring.hpp"

using miu::mkt::depth_ring;

TEST(ut_depth_ring, next) {
    char buf[4096] {};
    auto ring = depth_ring::make(buf, sizeof(buf), 8);

    // DO NOT TOUCH 0
    EXPECT_EQ(1U, ring->next());
    EXPECT_EQ(2U, ring->next());

    // wrapping
    for (auto i = ring->index(); i < ring->capacity() - 1; i++) {
        EXPECT_EQ(i + 1, ring->next());
    }

    // DO NOT TOUCH 0
    EXPECT_EQ(9U, ring->next());
}
