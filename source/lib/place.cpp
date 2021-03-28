
#include "place.hpp"

namespace miu::mkt {

place* place::make(void* buf,
                   uint32_t len,
                   ref::database const* db,
                   uint32_t quote_per_line,
                   uint32_t num_of_depth,
                   std::string_view name) {
    auto num_of_instruments = db->size();
    if (resolve_size(num_of_instruments, quote_per_line, num_of_depth) > len) {
        return nullptr;
    }

    auto ptr = new (buf) place { db->signature() };
    std::strncpy(ptr->_db_name, db->name().data(), sizeof(ptr->_name));

    ptr->_num_of_instruments = num_of_instruments;
    ptr->_quote_ring_size    = quote_ring::resolve_size(quote_per_line);
    ptr->_depth_ring_offset  = sizeof(place) + num_of_instruments * ptr->_quote_ring_size;

    std::strncpy(ptr->_name, name.data(), sizeof(ptr->_name));

    for (auto i = 0U; i < num_of_instruments; i++) {
        auto addr = ptr->get_quote_ring(i);
        auto ring = quote_ring::make(addr, ptr->_quote_ring_size, quote_per_line);
        ring->set_instrument_id(i);
        ring->set_time(time::clock::now());
    }

    auto depth_ring_size = depth_ring::resolve_size(num_of_depth);
    depth_ring::make((char*)ptr + ptr->_depth_ring_offset, depth_ring_size, num_of_depth);

    return ptr;
}

}    // namespace miu::mkt
