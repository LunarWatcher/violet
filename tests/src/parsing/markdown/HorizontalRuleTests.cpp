#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <violet/parsing/Markdown.hpp>

TEST_CASE("Horizontal rules should work") {
    SECTION("Standalone and trailing in file") {
        std::stringstream ss;
        ss << "---";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<hr/>"
        );
    }
}

/**
 * Violet-flavoured markdown does not support
 * ```markdown
 * Header
 * ------
 * ```
 * Because they suck to parse. Instead, `---` on a standalone line inside a paragraph should break that paragraph and
 * result in two paragraphs separated by an <hr>
 *
 * This particular feature is also not intentional, and is more of a regression test. For context, the paragraph parsing
 * mode is designed to break out if it detects a new major mode on a new line. This is to allow
 * ```markdown
 * Para text
 * * Bullet list
 * ```
 * To render properly. This also means that
 * ```markdown
 * Text
 * ---
 * ```
 * Breaks out of paragraph mode. A subsequent paragraph after `---` should then default back to paragraph parsing as a
 * new node.
 */
TEST_CASE("Horizontal rules should break out of paragraphs") {
    std::stringstream ss;
    ss << "This is a paragraph\n"
       << "---\n"
       << "And this is a new paragraph";

    REQUIRE(
        violet::Markdown::parse(ss)
        ==
        "<p>This is a paragraph</p>"
        "<hr/>"
        "<p>And this is a new paragraph</p>"
    );
}
