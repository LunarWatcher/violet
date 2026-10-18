#include "violet/parsing/Markdown.hpp"
#include "violet/parsing/markdown/DOMTree.hpp"
#include "violet/parsing/markdown/ParseAPI.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Angle bracket links", "[markdown][angle-bracket-links]") {
    std::stringstream ss;
    SECTION("Should correctly identify major modes") {
        std::vector<std::pair<std::string, bool>> cases {
            { "<https://example.com>", true },
            { "<http://example.com>", true },
            { "<gemini://example.gemini", false },
            { "<sub>", false },
            { "<http://>", true }, // Technically allowed but obviously pretty pointless,
            { "<https://>", true }, // Technically allowed but obviously pretty pointless,
        };

        for (const auto& [testCase, isParagraph] : cases) {
            INFO("testCase = " << testCase);
            violet::Markdown::DOMTree node(violet::Markdown::NodeType::DocumentRoot);
            std::stringstream ss;
            ss << testCase;
            auto result = violet::Markdown::resolveMajorMode(
                ss,
                &node
            ) == violet::Markdown::NodeType::Paragraph;

            REQUIRE(result == isParagraph);
        }
    }
    SECTION("should be parsed at the start of a paragraph") {
        ss << "<https://example.com>";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><a href="https://example.com">https://example.com</a></p>)"
        );
    }
    SECTION("should allow < escaping at the start of a paragraph") {
        ss << "\\<https://example.com\\>";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p>&lt;https://example.com&gt;</p>)"
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
    SECTION("should not be parsed inside other text if < is escaped") {
        ss << "text\\<https://example.com\\>text";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p>text&lt;https://example.com&gt;text</p>)"
        );
    }

    SECTION("should not fail agressively on unterminated links") {
        ss << "<https://example.com";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><https://example.com</p>)"
        );
    }

    SECTION("should not cause failures in unrelated HTML") {
        std::vector<std::string> cases {
            "<hr />",
            "<p></p>",
            "<p></p", // We don't validate HTML
            "<hr",
        };

        // Double-check the major mode identification (I don't think we have this test-case anywhere else?)
        for (const auto& testCase : cases) {
            std::stringstream ss;
            INFO("Test case: " << testCase);
            violet::Markdown::DOMTree node(violet::Markdown::NodeType::DocumentRoot);
            ss << testCase;
            auto result = violet::Markdown::resolveMajorMode(
                ss,
                &node
            );
            REQUIRE(result == violet::Markdown::NodeType::Template);

            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                "\n" + testCase
            );
        }
    }
}
