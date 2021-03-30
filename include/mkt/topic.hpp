#pragma once

#include <ref/price.hpp>
#include <ref/symbol.hpp>

namespace miu::mkt {

class quote_ring;
class depth_ring;

class topic {
  public:
    topic() = default;
    topic(quote_ring const*, depth_ring const*);

    auto operator!() const { return !_quotes; }
    operator bool() const { return !operator!(); }

    uint16_t id() const;
    ref::symbol symbol() const;

    ref::price bid() const;
    int32_t bid_vol() const;
    ref::price bid(uint32_t lev) const;
    int32_t bid_vol(uint32_t lev) const;

    ref::price ask() const;
    int32_t ask_vol() const;
    ref::price ask(uint32_t lev) const;
    int32_t ask_vol(uint32_t lev) const;

    ref::price last() const;
    int32_t last_vol() const;

    ref::price turnover() const;
    int32_t total_vol() const;
    int32_t open_interest() const;

    uint32_t refresh();

  private:
    quote_ring const* _quotes { nullptr };
    depth_ring const* _depths { nullptr };
    uint32_t _index { 0 };
};

}    // namespace miu::mkt
