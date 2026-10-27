#include <catch2/catch_test_macros.hpp>
#include "e2e/E2EFixture.hpp"

namespace {

TEST_CASE_METHOD(tests::E2EFixture, "Relative URLs should not be butchered", "[e2e][markdown]") {
    auto before = [&]() {
        loadSite()
            .withFile(
                "sub/path/webp-url.md",
                std::format(
                    "Hello world! This is a {}\nAnd this is a decoy newline: {}",
                    createDebugMarker("relativeUrl", "[webp url](file.webp)"),
                    createDebugMarker("absoluteUrl", "[absolute](/)")
                )
            )
            .withFile(
                "toplevel.md",
                std::format(
                    "Hello world! This is a {}\nAnd this is a decoy newline: {}",
                    createDebugMarker("relativeUrl", "[webp url](file.webp)"),
                    createDebugMarker("absoluteUrl", "[absolute](/)")
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
        REQUIRE(
            debug.at("absoluteUrl") == url("absolute", "/")
        );
    }
    SECTION("Non-empty prefix") {
        conf.prefix = "/prefix";
        before();
        autoBuild();
        requireOutputFileExists("toplevel.html");

        auto debug = getDebug(outputPath);

        REQUIRE(
            debug.at("relativeUrl") == url("webp url", "/prefix/sub/path/file.webp")
        );
        REQUIRE(
            debug.at("absoluteUrl") == url("absolute", "/prefix/")
        );
    }
}

}
