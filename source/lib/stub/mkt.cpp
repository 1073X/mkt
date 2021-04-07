
#include "stub/mkt.hpp"

#include <shm/tempfs.hpp>

#include "mkt/renewal.hpp"
#include "source/lib/mkt/place.hpp"

namespace miu::mkt {

stub::stub(uint32_t quote_per_line, uint32_t num_of_depth)
    : ref::stub() {
    auto size = place::resolve_size(db()->max_of_instrument(), quote_per_line, num_of_depth);
    _buf      = shm::buffer({ marker(), "mkt" }, size);
    place::make(_buf.data(), _buf.size(), db(), quote_per_line, num_of_depth, marker());
}

stub::~stub() {
    _buf.close();
    shm::tempfs::remove(marker(), "mkt");
}

topic stub::subscribe(uint32_t id) {
    auto place = place::open(_buf.data());

    auto quotes = place->get_quotes(id);
    if (!quotes) {
        return {};
    }

    quotes->observe();
    quotes->subscribe();
    return { quotes, place->get_depths() };
}

void stub::renew(uint32_t id, std::function<void(mkt::quote*)> const& cb) {
    auto place = place::open(_buf.data());
    renewal rnw { place->get_quotes(id), place->get_depths() };
    cb(rnw.quote());
}

void stub::renew(uint32_t id, std::function<void(mkt::quote*, mkt::depth*)> const& cb) {
    auto place = place::open(_buf.data());
    renewal rnw { place->get_quotes(id), place->get_depths() };
    cb(rnw.quote(), rnw.depth());
}

}    // namespace miu::mkt
