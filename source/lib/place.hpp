#pragma once

#include <cstring>    // std::strncpy
#include <ref/database.hpp>

#include "mkt/depth.hpp"

#include "depth_ring.hpp"
#include "quote_ring.hpp"

namespace miu::mkt {

class place {
  public:
    static uint32_t resolve_size(uint32_t max_of_instrument,
                                 uint32_t quote_per_line,
                                 uint32_t num_of_depth);

    static place* make(void* buf,
                       uint32_t len,
                       ref::database const* db,
                       uint32_t quote_per_line,
                       uint32_t num_of_depth,
                       std::string_view name);

    static place* open(void* buf) { return (place*)buf; }

  public:
    std::string_view name() const { return _name; }

    std::string_view db_name() const { return _db_name; }
    ref::signature db_sign() const { return _db_sign; }

    auto num_of_instrument() const { return _num_of_instrument; }

    quote_ring* get_quotes(uint32_t id);
    depth_ring* get_depths();

    auto padding() const { return _padding; }

  private:
    char _name[16] {};
    char _db_name[16] {};
    ref::signature _db_sign;
    uint32_t _num_of_instrument;
    uint32_t _quote_ring_size;
    uint32_t _depth_ring_offset;
    uint32_t _padding;
};
static_assert(sizeof(quote_ring) == sizeof(place));

}    // namespace miu::mkt
