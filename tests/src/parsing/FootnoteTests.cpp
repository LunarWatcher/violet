#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Footnotes should work") {
    std::stringstream ss;
    ss << R"(This is text[^1]

[^1]: This is, in fact, **text**)";
    REQUIRE(
        violet::Markdown::parse(ss)
        ==
        R"(<p>This is text<sup><a href="#fn-1">1</a></sup></p>)"
        R"(<div id="violet-footnotes">)"
            R"(<h2>Footnotes</h2>)"
            R"(<ol><li id="fn-1"><p>This is, in fact, <strong>text</strong></p></li></ol>)"
        R"(</div>)"
    );
}
