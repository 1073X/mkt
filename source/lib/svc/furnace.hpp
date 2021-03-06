#pragma once

#include <log/log.hpp>
#include <map>
#include <ref/database.hpp>
#include <svc/furnace.hpp>
#include <time/stamp.hpp>

#include "mkt/adapter.hpp"
#include "mkt/reg_var_str.hpp"
#include "mkt/version.hpp"

namespace miu::mkt {

namespace details {
    static auto create(cfg::settings const& settings,
                       svc::furnace* furnace,
                       std::map<std::string, ref::database>& dbs) {
        auto name           = settings.required<std::string>("name");
        auto db_name        = settings.required<std::string>("database");
        auto quote_per_line = settings.optional<uint32_t>("quote_per_line", 64);
        auto num_of_depth   = settings.optional<uint32_t>("num_of_depth", 8);

        if (dbs.find(db_name) == dbs.end()) {
            auto db = ref::database::open(db_name, shm::mode::READ);
            dbs.emplace(db_name, db);
        }

        std::unique_ptr<adapter> ptr { create_adapter() };
        ptr->make(name, furnace, &dbs[db_name], quote_per_line, num_of_depth);

        log::debug(+"mkt NEW adapter",
                   name,
                   +"quote_per_line",
                   quote_per_line,
                   +"num_of_depth",
                   num_of_depth);
        return ptr.release();
    }
}    // namespace details

class furnace : public svc::furnace {
  public:
    ~furnace() override { furnace::quench(); }

  public:    // implement svc::furnace
    void ignite(cfg::settings const& settings) override {
        reg_var_str();

        log::info(+"mkt VER", mkt::version());

        auto adapters = settings.required<cfg::settings>("adapters");
        for (auto i = 0U; i < adapters.size(); i++) {
            auto adapter_settings = adapters.required<cfg::settings>(i);
            _adapters.push_back(details::create(adapter_settings, this, _dbs));
            _adapters.back()->init(adapter_settings);
        }

        static time::delta const MIN_LAG { 1'000 };     // 1s
        static time::delta const MAX_LAG { 10'000 };    // 10s
        auto lag = settings.optional<time::delta>("lag", MIN_LAG);
        add_task("proc", std::min(std::max(lag, MIN_LAG), MAX_LAG), &furnace::proc, this);
    }

    void quench() override {
        for (auto adapter : _adapters) {
            adapter->uninit();
            delete adapter;
        }
        _adapters.clear();

        _dbs.clear();
    }

    auto connect(time::stamp now) {
        static std::chrono::seconds const INTERVAL { 30 };
        if (now - _last_connect_time > INTERVAL) {
            for (auto adapter : _adapters) {
                if (!adapter->is_connected()) {
                    log::debug(+"mkt CONNECT", adapter->name());
                    adapter->connect();
                }
            }
            _last_connect_time = now;
        }
    }

  private:
    void proc(job::status* st) {
        while (st->beat()) {
            auto now = time::clock::now();
            connect(now);

            for (auto adapter : _adapters) {
                adapter->discover();
            }
        }

        for (auto adapter : _adapters) {
            log::debug(+"mkt DISCONNECT", adapter->name());
            adapter->disconnect();
        }
    }

  private:
    std::map<std::string, ref::database> _dbs;
    std::vector<mkt::adapter*> _adapters;

    time::stamp _last_connect_time;
};

}    // namespace miu::mkt
