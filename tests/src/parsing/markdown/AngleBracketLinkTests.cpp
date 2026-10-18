#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Angle bracket links") {
    std::stringstream ss;
    SECTION("should be parsed at the start of a paragraph") {
        ss << "<https://example.com>";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><a href="https://example.com">https://example.com</a></p>)"
        );
    }
    SECTION("should be parsed inside other text") {
        ss << "text<https://example.com>text";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p>text<a href="https://example.com">https://example.com</a>text</p>)"
        );
    }
}
