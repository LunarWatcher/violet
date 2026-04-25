#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>
#include <sstream>

TEST_CASE("Sequential quotes", "[markdown]") {
    std::stringstream ss;
    ss << R"(> Quote 1

> Quote 2
)";
    SECTION("Parsing should be correct") {
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<blockquote><p>Quote 1</p></blockquote>"
            "<blockquote><p>Quote 2</p></blockquote>"
        );
    }
}

TEST_CASE("Multiline quotes") {
    SECTION("Simple case") {
        std::stringstream ss;
        ss << R"(> Quote 1
>
> Quote 2
)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<blockquote><p>Quote 1</p><p>Quote 2</p></blockquote>"
        );
    }
    SECTION("Mixed para end and para continuation") {
        std::stringstream ss;
        ss << R"(> Quote 1
> cont 1
>
> Quote 2
)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<blockquote><p>Quote 1 cont 1</p><p>Quote 2</p></blockquote>"
        );
    }
}
