
#include <svc/svc.hpp>

#include "furnace.hpp"

namespace miu::svc {
furnace* create() {
    return new mkt::furnace;
}
}    // namespace miu::svc
