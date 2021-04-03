
#include "mkt/adapter.hpp"
#include "source/lib/mkt/place.hpp"

namespace miu::mkt {

void adapter::make(std::string_view name,
                   ref::database const* db,
                   uint32_t quote_per_line,
                   uint32_t num_of_depth) {
    assert(!_buf && "adapter has been inited");
    assert(*db && "invalid database");

    _db = db;

    auto size = place::resolve_size(db->max_of_instrument(), quote_per_line, num_of_depth);
    _buf      = shm::buffer { { name, "mkt" }, size };
    place::make(_buf.data(), _buf.size(), db, quote_per_line, num_of_depth, name);
}

bool adapter::is_connected() const {
    return place::open(_buf.data())->is_connected();
}

void adapter::notify_connected() {
    place::open(_buf.data())->set_connected(1);
}

void adapter::notify_disconnected() {
    place::open(_buf.data())->set_connected(0);
}

void adapter::notify_subscribed(uint16_t instrument_id) {
    auto quotes = place::open(_buf.data())->get_quotes(instrument_id);
    quotes->subscribe();
}

renewal adapter::get_next(uint32_t instrument_id) {
    auto place  = place::open(_buf.data());
    auto quotes = place->get_quotes(instrument_id);
    if (!quotes) {
        return {};
    }
    auto depths = place->get_depths();
    return { quotes, depths };
}

renewal adapter::get_next(ref::symbol symbol) {
    auto inst = _db->find(symbol);
    if (!inst) {
        return {};
    }
    return get_next(inst.id());
}

renewal adapter::get_next_by_mkt_code(std::string_view code) {
    auto inst = _db->find_by_mkt_code(code);
    if (!inst) {
        return {};
    }
    return get_next(inst.id());
}

void adapter::discover() {
    auto place = place::open(_buf.data());
    if (place->is_connected()) {
        for (auto i = 0U; i < place->num_of_instrument(); i++) {
            auto quotes = place->get_quotes(i);
            if (quotes->is_observed() && !quotes->is_subscribed()) {
                subscribe(_db->find(i));
            }
        }
    } else {
        for (auto i = 0U; i < place->num_of_instrument(); i++) {
            auto quotes = place->get_quotes(i);
            quotes->unsubscribe();
        }
    }
}

}    // namespace miu::mkt
