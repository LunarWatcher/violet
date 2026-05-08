#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ToC feature tests") {
    std::stringstream ss;
    SECTION("Flat") {
        ss << R"(# Header
# Header 2
# Header 3)";

        auto res = std::move(
            violet::Markdown::parseWithContentPostprocessing(ss)
        );

        REQUIRE(
            res.parsedContents
            ==
            R"(<h1 id="header">Header</h1>)"
            R"(<h1 id="header-2">Header 2</h1>)"
            R"(<h1 id="header-3">Header 3</h1>)"
        );

        REQUIRE(
            res.tableOfContents
            ==
            "<ol>"
                R"(<li><a href="#header">Header</a></li>)"
                R"(<li><a href="#header-2">Header 2</a></li>)"
                R"(<li><a href="#header-3">Header 3</a></li>)"
            "</ol>"
        );
    }
    
    SECTION("Hierarchy") {
        ss << R"(# Header
## Header 2
### Header 3)";

        auto res = std::move(
            violet::Markdown::parseWithContentPostprocessing(ss)
        );

        REQUIRE(
            res.parsedContents
            ==
            R"(<h1 id="header">Header</h1>)"
            R"(<h2 id="header-2">Header 2</h2>)"
            R"(<h3 id="header-3">Header 3</h3>)"
        );

        REQUIRE(
            res.tableOfContents
            ==
            "<ol>"
                R"(<li><a href="#header">Header</a>)"
                    "<ol>"
                        R"(<li><a href="#header-2">Header 2</a>)"
                            "<ol>"
                                R"(<li><a href="#header-3">Header 3</a></li>)"
                            "</ol>"
                        "</li>"
                    "</ol>"
                "</li>"
            "</ol>"
        );
    }
    SECTION("Offset hierarchy") {
        ss << R"(## Header
### Header 2
#### Header 3)";

        auto res = std::move(
            violet::Markdown::parseWithContentPostprocessing(ss)
        );

        REQUIRE(
            res.parsedContents
            ==
            R"(<h2 id="header">Header</h2>)"
            R"(<h3 id="header-2">Header 2</h3>)"
            R"(<h4 id="header-3">Header 3</h4>)"
        );

        REQUIRE(
            res.tableOfContents
            ==
            "<ol>"
                R"(<li><a href="#header">Header</a>)"
                    "<ol>"
                        R"(<li><a href="#header-2">Header 2</a>)"
                            "<ol>"
                                R"(<li><a href="#header-3">Header 3</a></li>)"
                            "</ol>"
                        "</li>"
                    "</ol>"
                "</li>"
            "</ol>"
        );
    }
    SECTION("Invalid hierarchy") {
        ss << R"(# Header
### Header 2
)";

        auto res = std::move(
            violet::Markdown::parseWithContentPostprocessing(ss)
        );

        REQUIRE(
            res.parsedContents
            ==
            R"(<h1 id="header">Header</h1>)"
            R"(<h3 id="header-2">Header 2</h3>)"
        );

        REQUIRE(
            res.tableOfContents
            ==
            "<ol>"
                R"(<li><a href="#header">Header</a>)"
                    "<ol><li><ol>"
                        R"(<li><a href="#header-2">Header 2</a></li>)"
                    "</ol></li></ol>"
                "</li>"
            "</ol>"
        );
    }
}
