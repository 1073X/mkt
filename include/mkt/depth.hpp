#pragma once

#include <ref/price.hpp>

namespace miu::mkt {

class depth {
  public:
    static constexpr uint32_t max_levels() { return MAX_LEV; }

    auto bid(uint32_t i) const { return _bid[i]; }
    auto set_bid(uint32_t i, ref::price v) { _bid[i] = v; }

    auto bid_vol(uint32_t i) const { return _bid_vol[i]; }
    auto set_bid_vol(uint32_t i, int32_t v) { _bid_vol[i] = v; }

    auto ask(uint32_t i) const { return _ask[i]; }
    auto set_ask(uint32_t i, ref::price v) { _ask[i] = v; }

    auto ask_vol(uint32_t i) const { return _ask_vol[i]; }
    auto set_ask_vol(uint32_t i, int32_t v) { _ask_vol[i] = v; }

    auto padding() const { return _padding; }

  private:
    static uint32_t const MAX_LEV { 5 };

    ref::price _bid[MAX_LEV] {};
    ref::price _ask[MAX_LEV] {};
    int32_t _bid_vol[MAX_LEV] {};
    int32_t _ask_vol[MAX_LEV] {};
    int32_t _padding;
};
static_assert(CACHE_LINE * 2 == sizeof(depth));

}    // namespace miu::mkt
