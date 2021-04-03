#pragma once

#include <ref/symbol.hpp>
#include <time/stamp.hpp>

#include "mkt/quote.hpp"

#include "ring.hpp"

namespace miu::mkt {

namespace details {

    struct quote_ring {
        time::stamp time;
        ref::symbol symbol;
        uint32_t id;
        uint32_t mask;
        uint32_t index;
        uint32_t is_observed;
        uint32_t is_subscribed;
        uint32_t paddings[7];
    };

}    // namespace details

class quote_ring : public ring<details::quote_ring, quote> {
  public:
    static auto make(void* buf, uint32_t len, uint32_t capacity) {
        auto ptr = (quote_ring*)ring<details::quote_ring, quote>::make(buf, len, capacity);
        ptr->unsubscribe();
        return ptr;
    }

  public:
    auto time() const { return head()->time; }
    auto set_time(time::stamp v) { head()->time = v; }

    auto symbol() const { return head()->symbol; }
    auto set_symbol(ref::symbol v) { head()->symbol = v; }

    auto id() const { return head()->id; }
    auto set_id(uint32_t v) { head()->id = v; }

    auto is_observed() const { return head()->is_observed; }
    auto observe() { head()->is_observed = 1; }

    auto is_subscribed() const { return head()->is_subscribed; }
    auto subscribe() { head()->is_subscribed = 1; }
    void unsubscribe() { head()->is_subscribed = 0; }
};
static_assert(sizeof(quote) == sizeof(quote_ring));

}    // namespace miu::mkt
