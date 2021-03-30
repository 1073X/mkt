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
        uint32_t num_of_obs;
        uint32_t paddings[8];
    };

}    // namespace details

class quote_ring : public ring<details::quote_ring, quote> {
  public:
    static auto make(void* buf, uint32_t len, uint32_t capacity) {
        return (quote_ring*)ring<details::quote_ring, quote>::make(buf, len, capacity);
    }

  public:
    auto time() const { return head()->time; }
    auto set_time(time::stamp v) { head()->time = v; }

    auto symbol() const { return head()->symbol; }
    auto set_symbol(ref::symbol v) { head()->symbol = v; }

    auto id() const { return head()->id; }
    auto set_id(uint32_t v) { head()->id = v; }

    auto num_of_obs() const { return head()->num_of_obs; }
    auto subscribe() { head()->num_of_obs++; }

    template<typename CB>
    auto next(CB const& cb) {
        cb(at(index()));
        head()->index++;
    }
};
static_assert(sizeof(quote) == sizeof(quote_ring));

}    // namespace miu::mkt
