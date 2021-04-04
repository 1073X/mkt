#include <gmock/gmock.h>

#include <log/log.hpp>
#include <shm/tempfs.hpp>
#include <stub/ref.hpp>

#include "mkt/adapter.hpp"
#include "mkt/reg_var_str.hpp"
#include "source/lib/mkt/place.hpp"

struct ut_adapter : public testing::Test {
    struct adapter : public miu::mkt::adapter {
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

TEST_F(ut_adapter, create) {
    {
        struct adapter adapter;
        EXPECT_FALSE(adapter);
        adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);
        EXPECT_TRUE(adapter);

        EXPECT_EQ(stub.db(), adapter.database());
    }
    EXPECT_TRUE(miu::shm::tempfs::exists(stub.marker(), "mkt"));

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    EXPECT_EQ(stub.marker(), place->name());
    EXPECT_EQ(stub.db()->name(), place->db_name());
    EXPECT_EQ(stub.db()->signature(), place->db_sign());
    EXPECT_EQ(stub.db()->num_of_instrument(), place->num_of_instrument());
}

TEST_F(ut_adapter, get_next_by_id) {
    struct adapter adapter;
    adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);

    auto renewal = adapter.get_next(1);
    ASSERT_TRUE(renewal);
    EXPECT_EQ(1U, renewal.id());
    EXPECT_EQ(stub.db()->find(1).symbol(), renewal.symbol());

    EXPECT_FALSE(adapter.get_next(stub.db()->num_of_instrument()));
}

TEST_F(ut_adapter, get_next_by_symbol) {
    struct adapter adapter;
    adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);

    auto inst    = stub.db()->find(2);
    auto renewal = adapter.get_next(inst.symbol());
    ASSERT_TRUE(renewal);
    EXPECT_EQ(2U, renewal.id());
    EXPECT_EQ(inst.symbol(), renewal.symbol());

    EXPECT_FALSE(adapter.get_next(miu::ref::symbol("SSE/STOCK/123")));
}

TEST_F(ut_adapter, get_next_by_mkt_code) {
    struct adapter adapter;
    adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);

    auto inst    = stub.db()->find(2);
    auto renewal = adapter.get_next_by_mkt_code(inst.mkt_code());
    ASSERT_TRUE(renewal);
    EXPECT_EQ(2U, renewal.id());
    EXPECT_EQ(inst.symbol(), renewal.symbol());

    EXPECT_FALSE(adapter.get_next_by_mkt_code("not_exists"));
}

TEST_F(ut_adapter, notify_connected) {
    struct adapter adapter;
    adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);

    EXPECT_FALSE(adapter.is_connected());

    adapter.notify_connected();
    EXPECT_TRUE(adapter.is_connected());

    adapter.notify_disconnected();
    EXPECT_FALSE(adapter.is_connected());
}

TEST_F(ut_adapter, notify_subscribed) {
    struct adapter adapter;
    adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);
    adapter.notify_subscribed(1);

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    EXPECT_TRUE(place->get_quotes(1)->is_subscribed());
}

TEST_F(ut_adapter, discover) {
    struct adapter adapter;
    adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    place->set_connected(1);
    place->get_quotes(2)->observe();
    place->get_quotes(3)->observe();

    EXPECT_CALL(adapter, subscribe(testing::_)).Times(2);
    adapter.discover();
}

TEST_F(ut_adapter, discover_when_disconnected) {
    struct adapter adapter;
    adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    place->get_quotes(2)->observe();

    // should not subscribe to adapter implementation
    EXPECT_CALL(adapter, subscribe(testing::_)).Times(0);
    adapter.discover();
}

TEST_F(ut_adapter, discover_subscribed) {
    struct adapter adapter;
    adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);
    adapter.notify_subscribed(3);

    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    place->set_connected(1);
    place->get_quotes(1)->observe();
    place->get_quotes(3)->observe();

    EXPECT_CALL(adapter, subscribe(testing::_)).Times(1);    // only once for instrument 1
    adapter.discover();
}

TEST_F(ut_adapter, clean_subscribed) {
    struct adapter adapter;
    adapter.make(stub.marker(), nullptr, stub.db(), 16, 4);

    // suppose that we have 3 subscriptions
    adapter.notify_subscribed(3);
    adapter.notify_subscribed(2);
    adapter.notify_subscribed(1);

    // and then we try discovering when it is disconnected
    adapter.discover();

    // subscribed flags should be removed
    miu::shm::buffer buf { { stub.marker(), "mkt" }, miu::shm::mode::READ };
    auto place = miu::mkt::place::open(buf.data());
    for (auto i = 0; i < place->num_of_instrument(); i++) {
        EXPECT_FALSE(place->get_quotes(i)->is_subscribed());
    }
}
