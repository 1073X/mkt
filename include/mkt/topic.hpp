#pragma once

#include <com/to_string.hpp>
#include <ref/price.hpp>
#include <ref/symbol.hpp>
#include <time/stamp.hpp>

namespace miu::mkt {

class quote_ring;
class depth_ring;

class topic {
  public:
    topic() = default;
    topic(quote_ring*, depth_ring*);

    auto operator!() const { return !_quotes; }
    operator bool() const { return !operator!(); }

    uint32_t index() const;
    bool is_subscribed() const;

    uint32_t max_depth() const;

    uint16_t id() const;
    ref::symbol symbol() const;

    time::stamp local_time() const;
    time::stamp exchange_time() const;

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

  protected:
    quote_ring* _quotes { nullptr };
    depth_ring* _depths { nullptr };
    uint32_t _index { 0 };
};

}    // namespace miu::mkt

DEF_TO_STRING(miu::mkt::topic);
