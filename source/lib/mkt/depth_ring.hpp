#pragma once

#include <com/predict.hpp>

#include "mkt/depth.hpp"

#include "ring.hpp"

namespace miu::mkt {

namespace details {
    struct depth_ring {
        uint32_t mask;
        uint32_t index;
        uint32_t paddings[30];
    };
}    // namespace details

class depth_ring : public ring<details::depth_ring, depth> {
  public:
    static auto make(void* buf, uint32_t len, uint32_t capacity) {
        return (depth_ring*)ring<details::depth_ring, depth>::make(buf, len, capacity);
    }

    auto next() {
        while (!(++head()->index & head()->mask)) {
        }
        return head()->index;
    }
};
static_assert(sizeof(depth) == sizeof(depth_ring));

}    // namespace miu::mkt
