#include "catch2/catch_test_macros.hpp"
#include "violet/parsing/Markdown.hpp"
#include <sstream>

TEST_CASE("Ensure inline links are parsed inline", "[markdown][regression]") {
    SECTION("Content before and after") {
        std::stringstream ss;
        ss << "This is a line that contains [a url](//example.com) in the middle of text";

        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p>This is a line that contains <a href="//example.com">a url</a> in the middle of text</p>)"
        );
    }
}

TEST_CASE("Link translation", "[markdown]") {
    SECTION("Root-relative link") {
        std::stringstream ss;
        ss << "[Link](/test.md)";

        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><a href="/test.html">Link</a></p>)"
        );
    }

    SECTION(".md TLD should be untouched") {
        std::stringstream ss;
        ss << "[Link](//obsidian.md)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><a href="//obsidian.md">Link</a></p>)"
        );
    }

    SECTION("External pages should be untouched") {
        std::stringstream ss;
        ss << "[Link](//example.com/page.md)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><a href="//example.com/page.md">Link</a></p>)"
        );
    }
}
