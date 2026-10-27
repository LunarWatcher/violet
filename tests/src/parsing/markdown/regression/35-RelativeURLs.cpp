#include <catch2/catch_test_macros.hpp>
#include "e2e/E2EFixture.hpp"

namespace {

TEST_CASE_METHOD(tests::E2EFixture, "Relative URLs should not be butchered", "[e2e][markdown]") {
    auto before = [&]() {
        loadSite()
            .withFile(
                "sub/path/webp-url.md",
                std::format(
                    "Hello world! This is a {}\nAnd this is a decoy newline",
                    createDebugMarker("relativeUrl", "[webp url](file.webp)")
                )
            );
    };

    const static std::string outputPath = "sub/path/webp-url.html";
    SECTION("Empty prefix") {
        conf.prefix = "";
        before();
        autoBuild();
        requireOutputFileExists(outputPath);
        auto debug = getDebug(outputPath);

        REQUIRE(
            debug.at("relativeUrl") == url("webp url", "/sub/path/file.webp")
        );
    }
    SECTION("Non-empty prefix") {
        conf.prefix = "/prefix";
        before();
        autoBuild();
        requireOutputFileExists("sub/path/webp-url.html");

        auto debug = getDebug(outputPath);

        REQUIRE(
            debug.at("relativeUrl") == url("webp url", "/prefix/sub/path/file.webp")
        );
    }
}

}
