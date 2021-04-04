
#include "mkt/source.hpp"

#include <log/log.hpp>

#include "place.hpp"

namespace miu::mkt {

static auto do_subscribe(quote_ring* quotes, depth_ring* depths) {
    log::debug(+"mkt SUB",
               quotes->index(),
               quotes->time(),
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
    auto place = place::open(_buf.data());
    return place->db_name();
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

}    // namespace miu::mkt
