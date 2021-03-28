#pragma once

#include <cstring>    // std::strncpy
#include <ref/database.hpp>

#include "mkt/depth.hpp"

#include "depth_ring.hpp"
#include "quote_ring.hpp"

namespace miu::mkt {

class place {
  public:
    static auto resolve_size(uint32_t num_of_instruments,
                             uint32_t quote_per_line,
                             uint32_t num_of_depth) {
        return sizeof(place)
             //
             + num_of_instruments * quote_ring::resolve_size(quote_per_line)
             //
             + depth_ring::resolve_size(num_of_depth);
    }

    static place* make(void* buf,
                       uint32_t len,
                       ref::database const* db,
                       uint32_t quote_per_line,
                       uint32_t num_of_depth,
                       std::string_view name);

    static place* open(void* buf) { return (place*)buf; }

  private:
    place(ref::signature db_sign)
        : _db_sign(db_sign) {}

    auto paddings() { return _paddings; }

  public:
    std::string_view name() const { return _name; }

    std::string_view db_name() const { return _db_name; }
    ref::signature db_sign() const { return _db_sign; }

    quote_ring* get_quote_ring(uint32_t id) {
        if (id < _num_of_instruments) {
            auto addr = (char*)this + sizeof(place);
            return (quote_ring*)(addr + id * _quote_ring_size);
        }
        return nullptr;
    }

    depth* get_depth(uint32_t id) {
        auto addr = (char*)this + _depth_ring_offset;
        return ((depth_ring*)addr)->at(id);
    }

  private:
    char _name[16] {};
    char _db_name[16] {};
    ref::signature _db_sign;
    uint32_t _num_of_instruments;
    uint32_t _quote_ring_size;
    uint32_t _depth_ring_offset;
    uint32_t _paddings[1];
};
static_assert(sizeof(quote_ring) == sizeof(place));

}    // namespace miu::mkt
