#pragma once

#include <cstring>    // std::strncpy
#include <ref/database.hpp>

#include "mkt/depth.hpp"

#include "depth_ring.hpp"
#include "quote_ring.hpp"

namespace miu::mkt {

class place {
  public:
    static uint32_t resolve_size(uint16_t max_of_instrument,
                                 uint16_t quote_per_line,
                                 uint16_t num_of_depth);

    static place* make(void* buf,
                       uint16_t len,
                       ref::database const* db,
                       uint16_t quote_per_line,
                       uint16_t num_of_depth,
                       std::string_view name);

    static auto open(void* buf) { return (place*)buf; }
    static auto open(void const* buf) { return (place const*)buf; }

  public:
    std::string_view name() const { return _name; }

    std::string_view db_name() const { return _db_name; }
    ref::signature db_sign() const { return _db_sign; }

    auto num_of_instrument() const { return _num_of_instrument; }

    quote_ring* get_quotes(uint16_t id);
    depth_ring* get_depths();

    auto is_connected() const { return _is_connected != 0; }
    auto set_connected(uint16_t v) { _is_connected = v; }

    auto paddings() const { return _paddings; }

  private:
    char _name[16] {};
    char _db_name[16] {};
    ref::signature _db_sign;
    uint16_t _num_of_instrument;
    uint16_t _quote_ring_size;
    uint16_t _depth_ring_offset;
    uint16_t _is_connected;
    uint16_t _paddings[4];
};
static_assert(sizeof(quote_ring) == sizeof(place));

}    // namespace miu::mkt
