
#include "mkt/topic.hpp"

#include <cassert>
#include <sstream>

#include "depth_ring.hpp"
#include "quote_ring.hpp"

namespace miu::mkt {

#define QUOTE _quotes->at(_index)
#define DEPTH _depths->at(QUOTE->depth_id())

topic::topic(quote_ring* quotes, depth_ring* depths)
    : _quotes(quotes)
    , _depths(depths)
    , _index { _quotes->index() - 1 } {
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
    return QUOTE->depth_id() > 0 ? depth::max_levels() : 0;
}

time::stamp topic::local_time() const {
    return _quotes->local_time();
}

time::stamp topic::exchange_time() const {
    return QUOTE->exchange_time();
}

ref::price topic::bid() const {
    return QUOTE->bid();
}

ref::price topic::bid(uint32_t lev) const {
    return DEPTH->bid(lev);
}

int32_t topic::bid_vol(uint32_t lev) const {
    return DEPTH->bid_vol(lev);
}

int32_t topic::bid_vol() const {
    return QUOTE->bid_vol();
}

ref::price topic::ask() const {
    return QUOTE->ask();
}

ref::price topic::ask(uint32_t lev) const {
    return DEPTH->ask(lev);
}

int32_t topic::ask_vol(uint32_t lev) const {
    return DEPTH->ask_vol(lev);
}

int32_t topic::ask_vol() const {
    return QUOTE->ask_vol();
}

ref::price topic::last() const {
    return QUOTE->last();
}

int32_t topic::last_vol() const {
    return QUOTE->last_vol();
}

ref::price topic::turnover() const {
    return QUOTE->turnover();
}

int32_t topic::total_vol() const {
    return QUOTE->total_vol();
}

int32_t topic::open_interest() const {
    return QUOTE->open_interest();
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
