#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Unordered lists", "[markdown]") {
    auto impl = [&](const std::string& separator) {
        SECTION("Plain bullet lists with " + separator) {
            std::stringstream ss;
            ss << separator << " Bullet *1*\n"
               << separator << " Bullet **2**";
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                "<ul><li><p>Bullet <em>1</em></p></li><li><p>Bullet <strong>2</strong></p></li></ul>"
            );
        }
        SECTION("Bullet lists with multiple paragraphs using " + separator) {
            std::stringstream ss;
            ss << separator << " Bullet *1*\n"
               << "  Line continuation\n"
               << "\n"
               << "  extra paragraph\n"
               << separator << " Bullet **2**";
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                "<ul>"
                    "<li><p>Bullet <em>1</em> Line continuation</p>"
                        "<p>extra paragraph</p>"
                    "</li>"
                    "<li><p>Bullet <strong>2</strong></p></li>"
                "</ul>"
            );
        }
        SECTION("Bullet list with code using " + separator) {
            std::stringstream ss;
            ss << separator << " Test\n"
               << "  ```cpp\n"
               << "  code content\n"
               << "  ```\n"
               << separator << " Second point";
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                "<ul>"
                    "<li>"
                    "<p>Test</p>"
                        "<pre><code class=\"language-cpp\">code content</code></pre>"
                    "</li>"
                    "<li>"
                        "<p>Second point</p>"
                    "</li>"
                "</ul>"
            );
        }
        SECTION("Bullet list with code directly under bullet with " + separator) {
            std::stringstream ss;
            ss << separator << " ```cpp\n"
               << "  code content\n"
               << "  ```\n"
               << separator << " Second point";
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                "<ul>"
                "<li>"
                "<pre><code class=\"language-cpp\">code content</code></pre>"
                "</li>"
                "<li>"
                "<p>Second point</p>"
                "</li>"
                "</ul>"
            );
        }
    };
    impl("*");
    impl("-");
}

TEST_CASE("Mixed unordered lists", "[markdown]") {
    std::stringstream ss;
    ss << R"(* Bullet 1
- Bullet 2
  * Bullet 3
  - Bullet 4)";
    REQUIRE(
        violet::Markdown::parse(ss)
        ==
        "<ul>"
            "<li><p>Bullet 1</p></li>"
            "<li>"
                "<p>Bullet 2</p>"
                "<ul>"
                    "<li><p>Bullet 3</p></li>"
                    "<li><p>Bullet 4</p></li>"
                "</ul>"
            "</li>"
        "</ul>"
    );
}

TEST_CASE("Ordered lists", "[markdown]") {
    SECTION("Plain numbered lists") {
        std::stringstream ss;
        ss << "1. Bullet *1*\n"
           << "2. Bullet **2**";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ol><li><p>Bullet <em>1</em></p></li><li><p>Bullet <strong>2</strong></p></li></ol>"
        );
    }
    SECTION("Numbered list with code") {
        std::stringstream ss;
        ss << "1. Test\n"
           << "  ```cpp\n"
           << "  code content\n"
           << "  ```\n"
           << "2. Second point";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ol>"
                "<li>"
                    "<p>Test</p>"
                    "<pre><code class=\"language-cpp\">code content</code></pre>"
                "</li>"
                "<li>"
                    "<p>Second point</p>"
                "</li>"
            "</ol>"
        );
    }
}
