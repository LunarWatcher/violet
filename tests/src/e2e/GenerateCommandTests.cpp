#include "e2e/E2EFixture.hpp"
#include <catch2/catch_test_macros.hpp>

namespace {

TEST_CASE_METHOD(tests::E2EFixture, "When a prefix is set and --prefix", "[e2e]") {
    // technically already set, we just want to force this in case I don't want the default to be setting a prefix in
    // the future. This particular
    conf.prefix = "/prefix";
    loadSite();

    SECTION("is not passed on the command line") {
        run({
            "generate", "-d"
        });
        requireOutput();

        auto debug = getDebug();
        REQUIRE(debug.at("prefix") == "/prefix");
    }
    SECTION("is passed on the command line") {
        run({
            "generate", "-d", "--prefix", "/trans-rights-are-human-rights"
        });
        requireOutput();

        auto debug = getDebug();
        REQUIRE(debug.at("prefix") == "/trans-rights-are-human-rights");
    }

    SECTION("is set, but -l is passed, generate should error") {
        run({
            "generate", "-ld", "--prefix", "/override"
        }, true);
    }
}

}
