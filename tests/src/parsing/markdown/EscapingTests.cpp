#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Escaped URLs") {
    std::stringstream ss;
    SECTION("Non-URL") {
        ss << R"(\[whatever text content\])";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p>[whatever text content]</p>)"
        );
    }
    SECTION("Brackets in URL") {
        ss << R"([URL \[with brackets\]](https://example.com))";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><a href="https://example.com">URL [with brackets]</a></p>)"
        );
    }
}
