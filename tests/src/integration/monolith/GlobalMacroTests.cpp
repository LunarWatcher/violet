#include <catch2/catch_test_macros.hpp>

#include "fixtures/TestSiteFixture.hpp"

TEST_CASE_PERSISTENT_FIXTURE(
    tests::MonolithRealPrefix,
    "Global macros on the site should be loaded"
) {
    auto data = getDebug("macros/mrrp.html");

    REQUIRE(data.contains("mrrp"));
    REQUIRE(data.size() == 1);

    REQUIRE(data.at("mrrp") == "*paws at violet* mrrp meow!\n");
}

