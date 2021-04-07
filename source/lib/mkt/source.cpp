
#include "mkt/source.hpp"

#include <log/log.hpp>

#include "place.hpp"

namespace miu::mkt {

static auto do_subscribe(quote_ring* quotes, depth_ring* depths) {
    log::debug(+"mkt SUB",
               quotes->index(),
               quotes->local_time(),
               +"MAX:",
               quotes->capacity(),
               +"IDX:",
               quotes->index(),
               quotes->symbol());

    quotes->observe();
    return topic { quotes, depths };
}

source::source(std::string_view name)
    : _buf({ name, "mkt" }, shm::mode::RDWR) {
}

std::string_view source::db_name() const {
    return place::open(_buf.data())->db_name();
}

uint32_t source::num_of_instrument() const {
    return place::open(_buf.data())->num_of_instrument();
}

topic source::subscribe(uint32_t instrument_id) {
    if (_buf) {
        auto place  = place::open(_buf.data());
        auto quotes = place->get_quotes(instrument_id);
        if (quotes) {
            return do_subscribe(quotes, place->get_depths());
        }
    }
    return {};
}

topic source::subscribe(ref::symbol symbol) {
    if (symbol && _buf) {
        auto place = place::open(_buf.data());
        for (auto i = 0U; i < place->num_of_instrument(); i++) {
            auto quotes = place->get_quotes(i);
            assert(quotes);
            if (quotes->symbol() == symbol) {
                return do_subscribe(quotes, place->get_depths());
            }
        }
    }
    return {};
}

topic source::get(uint32_t instrument_id) {
    auto place  = place::open(_buf.data());
    auto quotes = place->get_quotes(instrument_id);
    if (quotes && quotes->is_subscribed()) {
        return { quotes, place->get_depths() };
    }
    return {};
}

}    // namespace miu::mkt
