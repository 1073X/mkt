#include <gtest/gtest.h>

#include <iostream>
#include <meta/info.hpp>

#include "mkt/version.hpp"

TEST(ut_version, version) {
    std::cout << miu::mkt::version() << std::endl;
    std::cout << miu::mkt::build_info() << std::endl;

    std::cout << miu::meta::info() << std::endl;
}
