#pragma once

#include <ref/price.hpp>
#include <time/stamp.hpp>

namespace miu::mkt {

class quote {
  public:
    auto time() const { return _time; }
    auto set_time(time::stamp v) { _time = v; }

    auto bid() const { return _bid; }
    auto set_bid(ref::price v) { _bid = v; }

    auto bid_vol() const { return _bid_vol; }
    auto set_bid_vol(int32_t v) { _bid_vol = v; }

    auto ask() const { return _ask; }
    auto set_ask(ref::price v) { _ask = v; }

    auto ask_vol() const { return _ask_vol; }
    auto set_ask_vol(int32_t v) { _ask_vol = v; }

    auto last() const { return _last; }
    auto set_last(ref::price v) { _last = v; }

    auto last_vol() const { return _last_vol; }
    auto set_last_vol(int32_t v) { _last_vol = v; }

    auto turnover() const { return _turnover; }
    auto set_turnover(ref::price v) { _turnover = v; }

    auto total_vol() const { return _total_vol; }
    auto set_total_vol(int32_t v) { _total_vol = v; }

    auto open_interest() const { return _open_interest; }
    auto set_open_interest(int32_t v) { _open_interest = v; }

    auto depth_id() const { return _depth_id; }
    auto set_depth_id(uint32_t v) { _depth_id = v; }

  private:
    time::stamp _time;
    ref::price _bid;
    ref::price _ask;
    ref::price _last;
    ref::price _turnover;
    int32_t _bid_vol { 0 };
    int32_t _ask_vol { 0 };
    int32_t _last_vol { 0 };
    int32_t _total_vol { 0 };
    int32_t _open_interest { 0 };
    uint32_t _depth_id { 0 };
};
static_assert(CACHE_LINE == sizeof(quote));

}    // namespace miu::mkt
