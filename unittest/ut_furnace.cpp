#include <gtest/gtest.h>

#include <cfg/json_source.hpp>
#include <shm/tempfs.hpp>
#include <stub/ref.hpp>
#include <svc/svc.hpp>

#include "mkt/version.hpp"
#include "source/lib/mkt/place.hpp"
#include "source/lib/svc/furnace.hpp"

struct engine : public miu::mkt::engine {
    void init(miu::cfg::settings const&) override { inited++; }
    void connect() override { connected++; }
    void subscribe(miu::ref::instrument) override {}
    void disconnect() override {}
    void uninit() override { uninitped++; }

    inline static uint32_t inited { 0 };
    inline static uint32_t connected { 0 };
    inline static uint32_t uninitped { 0 };
};

miu::mkt::engine* miu::mkt::create_engine() {
    return new ::engine {};
}

struct ut_furnace : public testing::Test {
    void SetUp() override {
        stub.db();    // lazy creating
        furnace.reset(miu::svc::create());
        miu::shm::tempfs::remove(stub.marker(), "mkt");
    }

    void TearDown() override {
        furnace.reset();
        miu::shm::tempfs::remove("A", stub.marker(), "mkt");
        miu::shm::tempfs::remove("B", stub.marker(), "mkt");
        miu::shm::tempfs::remove(stub.marker(), "mkt");
    }

    miu::ref::stub stub;
    std::unique_ptr<miu::svc::furnace> furnace;
};

TEST_F(ut_furnace, version) {
    EXPECT_EQ(miu::mkt::version(), furnace->version());
    EXPECT_EQ(miu::mkt::build_info(), furnace->build_info());
}

TEST_F(ut_furnace, create_engines) {
    miu::com::json spec;

    miu::com::json engine_a;
    engine_a["name"]           = "A." + stub.marker();
    engine_a["database"]       = stub.marker();
    engine_a["quote_per_line"] = 16;
    engine_a["num_of_quote"]   = 8;
    spec["engines"].push_back(engine_a);
    miu::com::json engine_b;
    engine_b["name"]           = "B." + stub.marker();
    engine_b["database"]       = stub.marker();
    engine_b["quote_per_line"] = 16;
    engine_b["num_of_quote"]   = 8;
    spec["engines"].push_back(engine_b);

    miu::cfg::json_source source { "spec", spec };
    miu::cfg::settings settings { &source };

    auto ptr = dynamic_cast<miu::mkt::furnace*>(furnace.get());
    ASSERT_NE(nullptr, ptr);

    ptr->ignite(settings);
    EXPECT_EQ(2U, engine::inited);

    EXPECT_TRUE(miu::shm::tempfs::exists("A", stub.marker(), "mkt"));
    EXPECT_TRUE(miu::shm::tempfs::exists("B", stub.marker(), "mkt"));

    ptr->finish();
    EXPECT_EQ(2U, engine::uninitped);
}

TEST_F(ut_furnace, connect) {
    using namespace std::chrono_literals;

    miu::com::json spec;

    miu::com::json engine;
    engine["name"]           = stub.marker();
    engine["database"]       = stub.marker();
    engine["quote_per_line"] = 16;
    engine["num_of_quote"]   = 8;
    spec["engines"].push_back(engine);

    miu::cfg::json_source source { "spec", spec };
    miu::cfg::settings settings { &source };

    auto ptr = dynamic_cast<miu::mkt::furnace*>(furnace.get());
    ptr->ignite(settings);

    auto now = miu::time::clock::now();
    ptr->connect(now);
    EXPECT_EQ(1U, engine::connected);

    ptr->connect(now + 1s);    // interval
    EXPECT_EQ(1U, engine::connected);
    ptr->connect(now + 31s);    // interval
    EXPECT_EQ(2U, engine::connected);

    {
        auto buf   = miu::shm::buffer { { stub.marker(), "mkt" }, miu::shm::mode::RDWR };
        auto place = miu::mkt::place::open(buf.data());
        place->set_connected(1);
    }

    ptr->connect(now + 5min);    // has connected
    EXPECT_EQ(2U, engine::connected);
}

