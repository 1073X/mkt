
#include "mkt/renewal.hpp"

#include "source/lib/mkt/depth_ring.hpp"
#include "source/lib/mkt/quote_ring.hpp"

namespace miu::mkt {

#define QUOTE _quotes->at(_index)

renewal::renewal(quote_ring* quotes, depth_ring* depths)
    : topic(quotes, depths) {
    _quotes->set_local_time(time::clock::now());
    _index = _quotes->index();
    QUOTE->set_depth_id(0);    // clear depth
}

renewal::~renewal() {
    if (_quotes) {
        const_cast<quote_ring*>(_quotes)->inc_index();
    }
}

quote* renewal::quote() {
    return _quotes->at(_index);
}

depth* renewal::depth() {
    if (!QUOTE->depth_id()) {
        auto depth_id = _depths->next();
        QUOTE->set_depth_id(depth_id);
    }

    return _depths->at(QUOTE->depth_id());
}

}    // namespace miu::mkt
