
#include "mkt/topic.hpp"

#include <cassert>
#include <sstream>

#include "depth_ring.hpp"
#include "quote_ring.hpp"

namespace miu::mkt {

topic::topic(quote_ring* quotes, depth_ring* depths)
    : _quotes(quotes)
    , _depths(depths)
    , _index { _quotes->index() - 1 } {
}

quote const* topic::quote() const {
    return _quotes->at(_index);
}

depth const* topic::depth() const {
    return _depths->at(quote()->depth_id());
}

uint32_t topic::index() const {
    return _quotes->index();
}

uint16_t topic::id() const {
    return _quotes->id();
}

ref::symbol topic::symbol() const {
    return _quotes->symbol();
}

bool topic::is_subscribed() const {
    return _quotes->is_subscribed();
}

uint32_t topic::max_depth() const {
    return quote()->depth_id() > 0 ? depth::max_levels() : 0;
}

time::stamp topic::local_time() const {
    return _quotes->local_time();
}

time::stamp topic::exchange_time() const {
    return quote()->exchange_time();
}

ref::price topic::bid() const {
    return quote()->bid();
}

ref::price topic::bid(uint32_t lev) const {
    return depth()->bid(lev);
}

int32_t topic::bid_vol(uint32_t lev) const {
    return depth()->bid_vol(lev);
}

int32_t topic::bid_vol() const {
    return quote()->bid_vol();
}

ref::price topic::ask() const {
    return quote()->ask();
}

ref::price topic::ask(uint32_t lev) const {
    return depth()->ask(lev);
}

int32_t topic::ask_vol(uint32_t lev) const {
    return depth()->ask_vol(lev);
}

int32_t topic::ask_vol() const {
    return quote()->ask_vol();
}

ref::price topic::last() const {
    return quote()->last();
}

int32_t topic::last_vol() const {
    return quote()->last_vol();
}

ref::price topic::turnover() const {
    return quote()->turnover();
}

int32_t topic::total_vol() const {
    return quote()->total_vol();
}

int32_t topic::open_interest() const {
    return quote()->open_interest();
}

uint32_t topic::refresh() {
    auto index = _index;
    _index     = _quotes->index() - 1;
    return _index - index;
}

}    // namespace miu::mkt

DEF_TO_STRING(miu::mkt::topic) {
    std::ostringstream ss;
    ss << v.id() << ' ' << to_string(v.symbol()) << ' ' << v.index();
    ss << ' ' << to_string(v.local_time()) << ' ' << to_string(v.exchange_time());
    ss << " [" << v.bid() << ' ' << v.bid_vol();
    ss << "] [" << v.ask() << ' ' << v.ask_vol();
    ss << "] [" << v.last() << ' ' << v.last_vol();
    ss << "] " << v.total_vol();
    ss << ' ' << v.turnover();
    ss << ' ' << v.open_interest();

    for (auto i = 0U; i < v.max_depth(); i++) {
        ss << '\n' << v.bid_vol(i) << '\t' << v.bid(i);
        ss << '\t' << v.ask(i) << '\t' << v.ask_vol(i);
    }

    return ss.str();
}
