#include <catch2/catch_test_macros.hpp>

#include "fixtures/TestSiteFixture.hpp"

TEST_CASE_PERSISTENT_FIXTURE(
    (tests::TestSiteFixture<tests::SiteVariant::TestMonolith, true>),
    "Verify that loadJson works in the same tree"
) {
    auto data = getDebug("json/load_data.html");
    REQUIRE(data.contains("equality"));
    REQUIRE(data.contains("relData"));
    REQUIRE(data.contains("hasTransRights"));

    REQUIRE(data.at("relData") == R"({"trans_rights_are_human_rights":true})");
    REQUIRE(data.at("equality") == "true");
    REQUIRE(data.at("hasTransRights") == "true");
}

TEST_CASE_PERSISTENT_FIXTURE(
    (tests::TestSiteFixture<tests::SiteVariant::TestMonolith, true>),
    "Verify that loadJson works in different trees with abspaths"
) {
    auto data = getDebug("json_altroot/load_data.html");
    REQUIRE(data.contains("absData"));
    REQUIRE(data.contains("hasTransRights"));

    REQUIRE(data.at("absData") == R"({"trans_rights_are_human_rights":true})");
    REQUIRE(data.at("hasTransRights") == "true");
}
