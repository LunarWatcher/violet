#include <catch2/catch_test_macros.hpp>

#include "catch2/matchers/catch_matchers.hpp"
#include "parsing/markdown/fixture/MarkdownFixture.hpp"

namespace {

TEST_CASE_METHOD(
    tests::MarkdownFixture,
    "Standalone images with direct links should render correctly",
    "[markdown][img]"
) {
    REQUIRE(
        render("![alt](https://example.com)")
        ==
        paragraph(img("alt", "https://example.com"))
    );
}

TEST_CASE_METHOD(
    tests::MarkdownFixture,
    "Images using <links> should handle the URL equivalently to non-image links",
    "[markdown][img]"
) {
    REQUIRE(
        render("![alt](<https://example.com>)")
        ==
        paragraph(img("alt", "https://example.com"))
    );
    REQUIRE(
        render("![alt](<https://example.com>)")
        ==
        paragraph(img("alt", "https://example.com"))
    );
}

TEST_CASE_METHOD(tests::MarkdownFixture, "Standalone images with reference") {
    REQUIRE(
        render(
            R"(![alt][1]

[1]: https://example.com)"
        )
        ==
        paragraph(img("alt", "https://example.com"))
    );
}

TEST_CASE_METHOD(
    tests::MarkdownFixture,
    "Standalone images with reference in URL outer anchor should render as an img in an a tag",
    "[markdown][img]"
) {
    REQUIRE(
        render(
            R"([![alt][1]](https://kagi.com)

[1]: https://example.com)"
        )
        ==
        paragraph(
            url(
                img("alt", "https://example.com"),
                "https://kagi.com"
            )
        )
    );
}

TEST_CASE_METHOD(tests::MarkdownFixture, "Images preceeded or proceeded by text should render with the text first", "[markdown][img][paragraph]") {
    REQUIRE(
        render("Text ![alt](https://example.com)")
        ==
        paragraph({
                "Text ",
                img("alt", "https://example.com")
            })
    );
    REQUIRE(
        render("![alt](https://example.com) and more text")
        ==
        paragraph({
                img("alt", "https://example.com"),
                " and more text"
            })
    );
    REQUIRE(
        render("Text ![alt](https://example.com) and more text")
        ==
        paragraph({
                "Text ",
                img("alt", "https://example.com"),
                " and more text",
            })
    );
}

TEST_CASE_METHOD(tests::MarkdownFixture, "Plain link references should not work for images") {
    REQUIRE_THROWS_WITH(
        render(
            R"(![link-ref]

[link-ref]: https://example.com)"
        ),
        "Expected '(' or '['"
    );
}

}
