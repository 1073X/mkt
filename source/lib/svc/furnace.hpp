#pragma once

#include <map>
#include <ref/database.hpp>
#include <svc/furnace.hpp>
#include <time/stamp.hpp>

#include "mkt/engine.hpp"
#include "mkt/version.hpp"

namespace miu::mkt {

namespace details {
    static auto create(cfg::settings const& settings, std::map<std::string, ref::database>& dbs) {
        auto name           = settings.required<std::string>("name");
        auto db_name        = settings.required<std::string>("database");
        auto quote_per_line = settings.optional<uint32_t>("quote_per_line", 64);
        auto num_of_depth   = settings.optional<uint32_t>("num_of_depth", 8);

        if (dbs.find(db_name) == dbs.end()) {
            auto db = ref::database::open(db_name, shm::mode::READ);
            dbs.emplace(db_name, db);
        }

        std::unique_ptr<engine> ptr { create_engine() };
        ptr->make(name, &dbs[db_name], quote_per_line, num_of_depth);
        return ptr.release();
    }
}    // namespace details

class furnace : public svc::furnace {
  public:
    ~furnace() { quench(); }

  public:    // implement svc::furnace
    std::string_view version() const override { return mkt::version(); }
    std::string_view build_info() const override { return mkt::build_info(); }

    void ignite(cfg::settings const& settings) override {
        auto engines = settings.required<cfg::settings>("engines");
        for (auto i = 0U; i < engines.size(); i++) {
            auto engine_settings = engines.required<cfg::settings>(i);
            _engines.push_back(details::create(engine_settings, _dbs));
            _engines.back()->init(engine_settings);
        }

        static time::delta const MIN_LAG { 1'000'000 };    // 1s
        auto lag = settings.optional<time::delta>("lag", MIN_LAG);
        add_task("proc", std::max(lag, MIN_LAG), &furnace::proc, this);
    }

    void quench() override {
        for (auto engine : _engines) {
            engine->uninit();
            delete engine;
        }
        _engines.clear();

        _dbs.clear();
    }

    auto connect(time::stamp now) {
        static std::chrono::seconds const INTERVAL { 30 };
        if (now - _last_connect_time > INTERVAL) {
            for (auto engine : _engines) {
                if (!engine->is_connected()) {
                    engine->connect();
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

            for (auto engine : _engines) {
                engine->discover();
            }
        }

        for (auto engine : _engines) {
            engine->disconnect();
        }
    }

  private:
    std::map<std::string, ref::database> _dbs;
    std::vector<mkt::engine*> _engines;

    time::stamp _last_connect_time;
};

}    // namespace miu::mkt
