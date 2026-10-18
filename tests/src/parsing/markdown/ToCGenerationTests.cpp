#include "violet/parsing/Markdown.hpp"
#include "violet/parsing/markdown/ContextProvidingNodes.hpp"
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
            R"(<div id="violet-toc">)"
            "<ol>"
                R"(<li><a href="#header">Header</a></li>)"
                R"(<li><a href="#header-2">Header 2</a></li>)"
                R"(<li><a href="#header-3">Header 3</a></li>)"
            "</ol>"
            "</div>"
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
            R"(<div id="violet-toc">)"
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
            "</div>"
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
            R"(<div id="violet-toc">)"
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
            "</div>"
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
            R"(<div id="violet-toc">)"
            "<ol>"
                R"(<li><a href="#header">Header</a>)"
                    "<ol><li><ol>"
                        R"(<li><a href="#header-2">Header 2</a></li>)"
                    "</ol></li></ol>"
                "</li>"
            "</ol>"
            "</div>"
        );
    }

    SECTION("Hierarchy with returns to previous levels") {
        ss << R"(# Header
## Header 2
## Header 3
### Header 4
## Header 5
# Header 6)";

        auto res = std::move(
            violet::Markdown::parseWithContentPostprocessing(ss)
        );
        REQUIRE(
            res.tableOfContents
            ==
            R"(<div id="violet-toc">)"
            "<ol>"
                "<li>"
                    R"(<a href="#header">Header</a>)"
                    "<ol>"
                        R"(<li><a href="#header-2">Header 2</a></li>)"
                        "<li>"
                            R"(<a href="#header-3">Header 3</a>)"
                            "<ol>"
                                "<li>"
                                    R"(<a href="#header-4">Header 4</a>)"
                                "</li>"
                            "</ol>"
                        "</li>"
                        R"(<li><a href="#header-5">Header 5</a></li>)"
                    "</ol>"
                "</li>"
                "<li>"
                    R"(<a href="#header-6">Header 6</a>)"
                "</li>"
            "</ol>"
            "</div>"
        );
    }

    SECTION("Regression: nested URLs should not take precedence when rendering") {
        ss << "# [Hello](https://example.com)";
        auto res = std::move(
            violet::Markdown::parseWithContentPostprocessing(ss)
        );

        // This may be redundant, but doesn't hurt
        REQUIRE(
            res.parsedContents
            ==
            // The ID isn't _great_, but that'll have to be handled later
            R"(<h1 id="hellohttpsexamplecom"><a href="https://example.com">Hello</a></h1>)"
        );
        REQUIRE(
            res.tableOfContents
            ==
            R"(<div id="violet-toc">)"
            "<ol>"
                "<li>"
                    R"(<a href="#hellohttpsexamplecom">Hello</a>)"
                "</li>"
            "</ol>"
            R"(</div>)"
        );
    }

    SECTION("Hierarchy with returns to previous levels and min level 2 should omit h1s") {
        ss << R"(# Header
## Header 2
## Header 3
### Header 4
## Header 5
# Header 6)";

        auto res = std::move(
            violet::Markdown::parseWithContentPostprocessing(
                ss,
                violet::Markdown::defaultTranslator,
                2
            )
        );
        REQUIRE(
            res.tableOfContents
            ==
            R"(<div id="violet-toc">)"
            "<ol>"
                R"(<li><a href="#header-2">Header 2</a></li>)"
                "<li>"
                    R"(<a href="#header-3">Header 3</a>)"
                    "<ol>"
                        "<li>"
                            R"(<a href="#header-4">Header 4</a>)"
                        "</li>"
                    "</ol>"
                "</li>"
                R"(<li><a href="#header-5">Header 5</a></li>)"
            "</ol>"
            "</div>"
        );
    }

    SECTION("Invalid hierarchy with min level 2 should collapse the ToC into a single level") {
        ss << R"(# Header
### Header 2
)";

        auto res = std::move(
            violet::Markdown::parseWithContentPostprocessing(
                ss,
                violet::Markdown::defaultTranslator,
                2
            )
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
            R"(<div id="violet-toc">)"
            "<ol>"
                R"(<li><a href="#header-2">Header 2</a></li>)"
            "</ol>"
            "</div>"
        );
    }
}
