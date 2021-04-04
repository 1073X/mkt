
#include "place.hpp"

#include <log/log.hpp>

namespace miu::mkt {

namespace details {
    auto is_empty(ref::signature const& sign) {
        for (auto i = 0U; i < ref::signature::size(); i++) {
            if (sign[i] != 0) {
                return false;
            }
        }
        return true;
    }
}    // namespace details

uint32_t place::resolve_size(uint16_t max_of_instrument,
                             uint16_t quote_per_line,
                             uint16_t num_of_depth) {
    return sizeof(place)
         //
         + max_of_instrument * quote_ring::resolve_size(quote_per_line)
         //
         + depth_ring::resolve_size(num_of_depth);
}

quote_ring* place::get_quotes(uint16_t id) {
    if (id < _num_of_instrument) {
        auto addr = (char*)this + sizeof(place);
        return (quote_ring*)(addr + id * _quote_ring_size);
    }
    return nullptr;
}

depth_ring* place::get_depths() {
    auto addr = (char*)this + _depth_ring_offset;
    return (depth_ring*)addr;
}

place* place::make(void* buf,
                   uint16_t len,
                   ref::database const* db,
                   uint16_t quote_per_line,
                   uint16_t num_of_depth,
                   std::string_view name) {
    // verifying
    if (resolve_size(db->max_of_instrument(), quote_per_line, num_of_depth) > len) {
        FATAL_ERROR("lack of space to make mkt", name);
    }

    auto quote_ring_size   = quote_ring::resolve_size(quote_per_line);
    auto depth_ring_offset = sizeof(place) + db->max_of_instrument() * quote_ring_size;

    auto ptr = (place*)buf;
    if (ptr->_depth_ring_offset > 0 && ptr->_depth_ring_offset != depth_ring_offset) {
        FATAL_ERROR("inconsistent mkt structure", name);
    }
    if (!details::is_empty(ptr->_db_sign) && ptr->_db_sign != db->signature()) {
        FATAL_ERROR("inconsistent mkt database", name, ptr->_db_name, db->name());
    }

    // setting fields
    std::strncpy(ptr->_name, name.data(), sizeof(ptr->_name));

    ptr->_db_sign = db->signature();
    std::strncpy(ptr->_db_name, db->name().data(), sizeof(ptr->_name));

    ptr->_num_of_instrument = db->num_of_instrument();
    ptr->_quote_ring_size   = quote_ring_size;
    ptr->_depth_ring_offset = depth_ring_offset;

    // setting quote lines
    log::debug(+"mkt MAKE QUOTE PER LINE", quote_per_line);
    for (auto i = 0U; i < db->num_of_instrument(); i++) {
        auto addr   = ptr->get_quotes(i);
        auto quotes = quote_ring::make(addr, ptr->_quote_ring_size, quote_per_line);
        auto symbol = db->find(i).symbol();
        quotes->set_id(i);
        quotes->set_symbol(symbol);
        quotes->set_local_time(time::clock::now());
        log::debug(+"mkt MAKE QUOTE", i, quotes->local_time(), +"IDX:", quotes->index(), symbol);
    }

    // setting depth ring
    auto depth_ring_addr = (char*)ptr + ptr->_depth_ring_offset;
    auto depth_ring_size = depth_ring::resolve_size(num_of_depth);
    auto depths          = depth_ring::make(depth_ring_addr, depth_ring_size, num_of_depth);
    log::debug(+"mkt MAKE DEPTH MAX:", depths->capacity(), +"IDX:", depths->index());

    return ptr;
}

}    // namespace miu::mkt
