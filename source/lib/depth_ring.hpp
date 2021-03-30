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

    template<typename CB>
    auto next(CB const& callback) {
        auto index = head()->index;
        index += LIKELY(index & head()->mask) ? 1 : 2;
        callback(at(index));
        head()->index = index;
        return head()->index - 1;
    }
};
static_assert(sizeof(depth) == sizeof(depth_ring));

}    // namespace miu::mkt
