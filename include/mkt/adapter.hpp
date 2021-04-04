#pragma once

#include <cfg/settings.hpp>
#include <ref/database.hpp>
#include <shm/buffer.hpp>
#include <svc/furnace.hpp>

#include "renewal.hpp"

namespace miu::mkt {

class adapter {
  public:
    adapter()          = default;
    virtual ~adapter() = default;

    void make(std::string_view name,
              svc::furnace*,
              ref::database const*,
              uint32_t quote_per_line,
              uint32_t num_of_depth);

    auto operator!() const { return !_buf; }
    operator bool() const { return !operator!(); }

    std::string_view name() const;

  public:
    auto database() const { return _database; }

    renewal get_next(uint32_t instrument_id);
    renewal get_next(ref::symbol);
    renewal get_next_by_mkt_code(std::string_view);

  public:
    bool is_connected() const;
    void notify_connected();
    void notify_disconnected();

    void notify_subscribed(uint16_t instrument_id);

    template<typename... ARGS>
    auto add_task(ARGS&&... args) {
        assert(_furnace != nullptr);
        return _furnace->add_task(std::forward<ARGS>(args)...);
    }

  public:
    virtual void init(cfg::settings const&) = 0;
    virtual void connect()                  = 0;
    virtual void subscribe(ref::instrument) = 0;
    virtual void disconnect()               = 0;
    virtual void uninit()                   = 0;

    void discover();    // discovering new subscriptions

  private:
    shm::buffer _buf;
    svc::furnace* _furnace { nullptr };
    ref::database const* _database { nullptr };
};

extern adapter* create_adapter();

}    // namespace miu::mkt
