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

    topic subscribe(uint32_t instrument_id);
    topic subscribe(ref::symbol);

  private:
    shm::buffer _buf;
};

}    // namespace miu::mkt