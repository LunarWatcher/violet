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
