#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include "parsing/markdown/fixture/MarkdownFixture.hpp"

TEST_CASE("~~strike~~ should work") {
    std::stringstream ss;
    SECTION("Standalone") {
        ss << "~~content~~";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<p><s>content</s></p>"
        );
    }
    SECTION("In paragraph") {
        ss << "Text ~~content~~ here";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<p>Text <s>content</s> here</p>"
        );
    }

}

TEST_CASE_METHOD(tests::MarkdownFixture, "~~~del~~~ should work") {
    std::stringstream ss;
    SECTION("Standalone") {
        ss << "~~~content~~~";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            paragraph(
                "<del>content</del>"
            )
        );
    }
    SECTION("In paragraph") {
        ss << "Text ~~~content~~~ here";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            paragraph(
                "Text <del>content</del> here"
            )
        );
    }
}
