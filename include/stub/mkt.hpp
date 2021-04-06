#pragma once

#include <functional>
#include <stub/ref.hpp>

#include "mkt/depth.hpp"
#include "mkt/quote.hpp"
#include "mkt/topic.hpp"

namespace miu::mkt {

class stub : public ref::stub {
  public:
    stub(uint32_t quote_per_line = 16, uint32_t num_of_depth = 4);
    virtual ~stub();

    topic subscribe(uint32_t);

    void renew(uint32_t, std::function<void(mkt::quote*)> const&);
    void renew(uint32_t, std::function<void(mkt::quote*, mkt::depth*)> const&);

  private:
    shm::buffer _buf;
};

}    // namespace miu::mkt
