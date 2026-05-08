#include "violet/parsing/markdown/DocumentContext.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test ID conversion") {
    violet::Markdown::DocumentContext ctx(nullptr);

    SECTION("A")

    SECTION("Non-latin") {
        REQUIRE(ctx.idify("") == "-1");
        REQUIRE(ctx.idify("여우") == "-2");

        REQUIRE(ctx.idify("hiæøå") == "hi");
        REQUIRE(ctx.idify("hiæøå") == "hi-1");
    }

    SECTION("Latin") {
        REQUIRE(ctx.idify("test") == "test");

        for (size_t i = 0; i < 10; ++i) {
            REQUIRE(ctx.idify("test") == std::format("test-{}", i + 1));
        }
    }

    SECTION("Numbers") {
        REQUIRE(ctx.idify("test1234") == "test1234");
    }
}
