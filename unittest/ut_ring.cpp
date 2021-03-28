#include <gtest/gtest.h>

#include "source/lib/ring.hpp"

struct ut_ring : public testing::Test {
    struct head {
        uint32_t mask;
        uint32_t index;
    };

    struct item {
        uint32_t value;
        uint32_t padding;
    };

    char buf[4096] {};

    using ring_base = miu::mkt::ring<head, item>;
    class ring_type : public ring_base {
      public:
        static ring_type* make(void* buf, uint32_t len, uint32_t capacity) {
            return (ring_type*)ring_base::make(buf, len, capacity);
        }
    };
    ring_type* ring = ring_type::make(buf, sizeof(buf), 16);
};

TEST_F(ut_ring, resolve_size) {
    auto exp = sizeof(head) + sizeof(item) * 16;
    EXPECT_EQ(exp, ring_type::resolve_size(16));

    EXPECT_ANY_THROW(ring_type::resolve_size(15));
    EXPECT_ANY_THROW(ring_type::resolve_size(0));
}

TEST_F(ut_ring, default) {
    EXPECT_EQ(16U, ring->capacity());
    EXPECT_EQ(0U, ring->index());

    EXPECT_ANY_THROW(ring_type::make(buf, 0, 16));
    EXPECT_ANY_THROW(ring_type::make(buf, sizeof(buf), 15));
}

TEST_F(ut_ring, get) {
    EXPECT_EQ(buf + sizeof(head), (const char*)ring->at(0));
    EXPECT_EQ(buf + sizeof(head) + sizeof(item), (const char*)ring->at(1));

    // wrapping
    EXPECT_EQ(buf + sizeof(head), (const char*)ring->at(16));
    EXPECT_EQ(buf + sizeof(head) + sizeof(item), (const char*)ring->at(17));
}

TEST_F(ut_ring, next) {
    ring->next([](auto item) { item->value = 1; });
    EXPECT_EQ(1U, ring->index());
    EXPECT_EQ(1, ring->at(0)->value);

    for (auto i = 1U; i < ring->capacity(); i++) {
        ring->next([i](auto item) { item->value = i + 1; });
    }

    ring->next([](auto item) { item->value = 99; });
    EXPECT_EQ(17U, ring->index());
    EXPECT_EQ(99, ring->at(0)->value);
}
