#pragma once

#include "topic.hpp"

namespace miu::mkt {

class renewal : public topic {
  public:
    renewal() = default;
    renewal(quote_ring*, depth_ring*);
    renewal(renewal const&) = delete;
    auto operator=(renewal const&) = delete;
    ~renewal();

    class quote* quote();
    class depth* depth();
};

}    // namespace miu::mkt
