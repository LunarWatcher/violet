#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <utils/LinkableFeatureParityTestCaseGenerator.hpp>

namespace {

LINKABLE_FEATURE_PARITY_TEST_CASE("Parentheses in links should be handled") {
    SECTION("Balanced") {
        REQUIRE(
            render(
                getMarkdown("link", "https://en.wikipedia.org/wiki/Violet_(color)")
            )
            ==
            paragraph(
                getHtmlForLinkable("link", "https://en.wikipedia.org/wiki/Violet_(color)")
            )
        );
        REQUIRE(
            render(
                getMarkdown("link", "https://en.wikipedia.org/wiki/Violet_(color)")
                + ")"
            )
            ==
            paragraph({
                    getHtmlForLinkable("link", "https://en.wikipedia.org/wiki/Violet_(color)"),
                    ")"
                })
        );
        REQUIRE(
            render(
                getMarkdown("link", "https://en.wikipedia.org/wiki/Violet_(color)")
                    + " with text)"
            )
            ==
            paragraph({
                    getHtmlForLinkable("link", "https://en.wikipedia.org/wiki/Violet_(color)"),
                    " with text)"
                })
        );
    }
    SECTION("Unbalanced") {
        std::string err =
            "Unclosed URL. Hint: if your URL contains unbalanced parentheses and it's meant to "
            "be unbalanced, use an escaped link (`[text](<https://en.wikipedia.org/wiki/Violet_(color>)`). Otherwise, make "
            "sure you actually closed this URL correctly.";
        REQUIRE_THROWS_WITH(
            render(
                getMarkdown("link", "https://en.wikipedia.org/wiki/Violet_(color")
            ),
            err
        );
        REQUIRE_THROWS_WITH(
            render(
                getMarkdown("link", "https://en.wikipedia.org/wiki/Violet_(color")
                + " with text"
            ),
            err
        );
        REQUIRE_THROWS_WITH(
            render(
                getMarkdown("link", "https://en.wikipedia.org/wiki/Violet_(color")
                + "with text"
            ),
            err
        );
    }
    SECTION("Unbalanced with trailing text that contains a parenthesis") {
        REQUIRE(
            render(
                getMarkdown("link", "https://en.wikipedia.org/wiki/Violet_(color")
                + "with text)"
            )
            ==
            paragraph(
                getHtmlForLinkable("link", "https://en.wikipedia.org/wiki/Violet_(color)with text")
            )
        );
    }
}

LINKABLE_FEATURE_PARITY_TEST_CASE("Literal URLs") {
    REQUIRE(
        render(
            getMarkdown(
                "link", "<https://en.wikipedia.org/wiki/Violet_(color>"
            )
        )
        ==
        paragraph(
            getHtmlForLinkable("link", "https://en.wikipedia.org/wiki/Violet_(color")
        )
    );
    REQUIRE_THROWS(
        render(
            "[link](<https://en.wikipedia.org/wiki/Violet_(color>"
        )
    );
    REQUIRE_THROWS(
        render("[link](<https://en.wikipedia.org/wiki/Violet_(color))")
    );
}

LINKABLE_FEATURE_PARITY_TEST_CASE("Ensure a linkable doesn't displace content", "[regression]") {
    REQUIRE(
        render(
            std::format(
                "This is a line that contains {} in the middle of text",
                getMarkdown("a url or image", "//example.com")
            )
        )
        ==
        paragraph({
                "This is a line that contains ",
                getHtmlForLinkable("a url or image", "//example.com"),
                " in the middle of text"
            })
    );
    REQUIRE(
        render(
            std::format(
                "{} with text after",
                getMarkdown("a url or image", "//example.com")
            )
        )
        ==
        paragraph({
                getHtmlForLinkable("a url or image", "//example.com"),
                " with text after"
            })
    );
    REQUIRE(
        render(
            std::format(
                "This is a line that starts with text and then contains {}",
                getMarkdown("a url or image", "//example.com")
            )
        )
        ==
        paragraph({
                "This is a line that starts with text and then contains ",
                getHtmlForLinkable("a url or image", "//example.com"),
            })
    );
}

LINKABLE_FEATURE_PARITY_TEST_CASE("Link references should work") {
    REQUIRE(
        render(
            getMarkdown("Hello world", "cb", true)
            + "\n\n"
            + "[cb]: https://lunarwatcher.codeberg.page"
        )
        ==
        paragraph(
            getHtmlForLinkable("Hello world", "https://lunarwatcher.codeberg.page")
        )
    );
}


LINKABLE_FEATURE_PARITY_TEST_CASE("Link translation and renaming") {
    SECTION("Root-relative link") {
        REQUIRE(
            render(
                getMarkdown("Link", "/My page.md")
            )
            ==
            paragraph(
                getHtmlForLinkable("Link", "/my-page.html")
            )
        );
        REQUIRE(
            render(
                getMarkdown("Link", "/My page.md"),
                "/prefix"
            )
            ==
            paragraph(
                getHtmlForLinkable("Link", "/prefix/my-page.html")
            )
        );
        REQUIRE(
            render(
                getMarkdown("Link", "/My page.md"),
                "/prefix",
                "nested/page.md"
            )
            ==
            paragraph(
                getHtmlForLinkable("Link", "/prefix/my-page.html")
            )
        );
    }
    SECTION("Non-markdown files should not be converted, but should be URL-encoded") {
        REQUIRE(
            render(
                getMarkdown("Link", "/My image.png")
            )
            ==
            paragraph(
                getHtmlForLinkable("Link", "/My%20image.png")
            )
        );
        REQUIRE(
            render(
                getMarkdown("Link", "/My æøåimage.html")
            )
            ==
            paragraph(
                getHtmlForLinkable("Link", "/My%20%c3%a6%c3%b8%c3%a5image.html")
            )
        );
    }

    SECTION(".md TLD should be untouched") {
        REQUIRE(
            render(
                getMarkdown("Link", "//obsidian.md")
            )
            ==
            paragraph(
                getHtmlForLinkable("Link", "//obsidian.md")
            )
        );
    }

    SECTION("External pages should be untouched") {
        REQUIRE(
            render(
                getMarkdown("Link", "//example.com/page.md")
            )
            ==
            paragraph(
                getHtmlForLinkable("Link", "//example.com/page.md")
            )
        );
    }
}


}
