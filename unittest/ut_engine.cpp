#include <gmock/gmock.h>

#include <log/log.hpp>
#include <shm/tempfs.hpp>
#include <stub/ref.hpp>

#include "mkt/engine.hpp"
#include "mkt/reg_var_str.hpp"
#include "source/lib/mkt/place.hpp"

struct ut_engine : public testing::Test {
    struct engine : public miu::mkt::engine {
        MOCK_METHOD(void, subscribe, (miu::ref::instrument), (override));
        MOCK_METHOD(void, init, (miu::cfg::settings const&), (override));
        MOCK_METHOD(void, uninit, (), (override));
        MOCK_METHOD(void, connect, (), (override));
        MOCK_METHOD(void, disconnect, (), (override));
    };

    void SetUp() override {
        miu::mkt::reg_var_str();
        // miu::log::reset(miu::log::severity::DEBUG, 1024);
    }

    void TearDown() override {
        miu::shm::tempfs::remove(stub.marker(), "mkt");
        miu::log::dump();
    }

    miu::ref::stub stub;
};

TEST_F(ut_engine, create) {
    {
        struct engine engine;
        EXPECT_FALSE(engine);
        engine.make(stub.marker(), stub.db(), 16, 4);
        EXPECT_TRUE(engine);

        EXPECT_EQ(stub.db(), engine.database());
    }
    EXPECT_TRUE(miu::shm::tempfs::exists(stub.marker(), "mkt"));

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    EXPECT_EQ(stub.marker(), place->name());
    EXPECT_EQ(stub.db()->name(), place->db_name());
    EXPECT_EQ(stub.db()->signature(), place->db_sign());
    EXPECT_EQ(stub.db()->num_of_instrument(), place->num_of_instrument());
}

TEST_F(ut_engine, get_next_by_id) {
    struct engine engine;
    engine.make(stub.marker(), stub.db(), 16, 4);

    auto renewal = engine.get_next(1);
    ASSERT_TRUE(renewal);
    EXPECT_EQ(1U, renewal.id());
    EXPECT_EQ(stub.db()->find(1).symbol(), renewal.symbol());

    EXPECT_FALSE(engine.get_next(stub.db()->num_of_instrument()));
}

TEST_F(ut_engine, get_next_by_symbol) {
    struct engine engine;
    engine.make(stub.marker(), stub.db(), 16, 4);

    auto inst    = stub.db()->find(2);
    auto renewal = engine.get_next(inst.symbol());
    ASSERT_TRUE(renewal);
    EXPECT_EQ(2U, renewal.id());
    std::cout << miu::com::to_string(inst.symbol()) << std::endl;
    std::cout << miu::com::to_string(renewal.symbol()) << std::endl;
    EXPECT_EQ(inst.symbol(), renewal.symbol());

    EXPECT_FALSE(engine.get_next(miu::ref::symbol("SSE/STOCK/123")));
}

TEST_F(ut_engine, get_next_by_mkt_code) {
    struct engine engine;
    engine.make(stub.marker(), stub.db(), 16, 4);

    auto inst    = stub.db()->find(2);
    auto renewal = engine.get_next_by_mkt_code(inst.mkt_code());
    ASSERT_TRUE(renewal);
    EXPECT_EQ(2U, renewal.id());
    std::cout << miu::com::to_string(inst.symbol()) << std::endl;
    std::cout << miu::com::to_string(renewal.symbol()) << std::endl;
    EXPECT_EQ(inst.symbol(), renewal.symbol());

    EXPECT_FALSE(engine.get_next_by_mkt_code("not_exists"));
}

TEST_F(ut_engine, notify_connected) {
    struct engine engine;
    engine.make(stub.marker(), stub.db(), 16, 4);

    EXPECT_FALSE(engine.is_connected());

    engine.notify_connected();
    EXPECT_TRUE(engine.is_connected());

    engine.notify_disconnected();
    EXPECT_FALSE(engine.is_connected());
}

TEST_F(ut_engine, notify_subscribed) {
    struct engine engine;
    engine.make(stub.marker(), stub.db(), 16, 4);
    engine.notify_subscribed(1);

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    EXPECT_TRUE(place->get_quotes(1)->is_subscribed());
}

TEST_F(ut_engine, discover) {
    struct engine engine;
    engine.make(stub.marker(), stub.db(), 16, 4);

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    place->set_connected(1);
    place->get_quotes(2)->observe();
    place->get_quotes(3)->observe();

    EXPECT_CALL(engine, subscribe(testing::_)).Times(2);
    engine.discover();
}

TEST_F(ut_engine, discover_when_disconnected) {
    struct engine engine;
    engine.make(stub.marker(), stub.db(), 16, 4);

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    place->get_quotes(2)->observe();

    // should not subscribe to engine implementation
    EXPECT_CALL(engine, subscribe(testing::_)).Times(0);
    engine.discover();
}

TEST_F(ut_engine, discover_subscribed) {
    struct engine engine;
    engine.make(stub.marker(), stub.db(), 16, 4);
    engine.notify_subscribed(3);

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    place->set_connected(1);
    place->get_quotes(1)->observe();
    place->get_quotes(3)->observe();

    EXPECT_CALL(engine, subscribe(testing::_)).Times(1);    // only once for instrument 1
    engine.discover();
}

TEST_F(ut_engine, clean_subscribed) {
    struct engine engine;
    engine.make(stub.marker(), stub.db(), 16, 4);

    // suppose that we have 3 subscriptions
    engine.notify_subscribed(3);
    engine.notify_subscribed(2);
    engine.notify_subscribed(1);

    // and then we try discovering when it is disconnected
    engine.discover();

    // subscribed flags should be removed
    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    for (auto i = 0; i < place->num_of_instrument(); i++) {
        EXPECT_FALSE(place->get_quotes(i)->is_subscribed());
    }
}
