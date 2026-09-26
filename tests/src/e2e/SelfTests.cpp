#include "e2e/E2EFixture.hpp"
#include "fixtures/LoadWorkspace.hpp"
#include <catch2/catch_test_macros.hpp>

namespace {

TEST_CASE_METHOD(tests::E2EFixture, "Self-test the E2E fixture", "[e2e]") {
    conf.prefix = "/prefix";
    loadSite();
    run({
        "generate"
    });

    requireOutput();

    auto debug = getDebug();
    REQUIRE(debug.contains("prefix"));
    REQUIRE(debug.at("prefix") == conf.prefix);
}

}
