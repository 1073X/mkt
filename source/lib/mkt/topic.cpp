
#include "mkt/topic.hpp"

#include <cassert>

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

uint16_t topic::id() const {
    return _quotes->id();
}

ref::symbol topic::symbol() const {
    return _quotes->symbol();
}

bool topic::is_subscribed() const {
    return _quotes->is_subscribed();
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
