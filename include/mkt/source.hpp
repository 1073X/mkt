#pragma once

#include <shm/buffer.hpp>

#include "topic.hpp"

namespace miu::mkt {

class source {
  public:
    source() = default;
    explicit source(std::string_view);

    auto operator!() const { return !_buf; }
    operator bool() const { return !operator!(); }

    std::string_view name() const;
    std::string_view db_name() const;
    uint32_t num_of_instrument() const;

    topic subscribe(uint32_t instrument_id);
    topic subscribe(ref::symbol);

    topic get(uint32_t instrument_id);    // get subscribed topic

  private:
    shm::buffer _buf;
};

}    // namespace miu::mkt
