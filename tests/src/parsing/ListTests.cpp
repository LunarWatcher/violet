#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Unordered lists", "[markdown]") {
    auto impl = [&](const std::string& separator, char checklistChar) {
        std::string d = std::format(
            "{} (checklist mode: {})",
            separator,
            checklistChar != '\0' ? std::string(checklistChar, 1) : "off"
        );
        std::string op = std::format(
            "{}{}",
            separator,
            checklistChar != '\0' ? std::format(" [{}]", checklistChar) : ""
        );
        std::string prefix = "";
        if (checklistChar != '\0') {
            prefix = "<input type=\"checkbox\" disabled";
            if (checklistChar != ' ') {
                prefix += " checked";
            }
            prefix += "/>";
        }
        SECTION("Plain bullet lists with " + d) {
            std::stringstream ss;
            ss << op << " Bullet *1*\n"
               << op << " Bullet **2**";
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                "<ul><li>" + prefix + "<p>Bullet <em>1</em></p></li><li>" + prefix + "<p>Bullet <strong>2</strong></p></li></ul>"
            );
        }
        SECTION("Bullet lists with multiple paragraphs using " + d) {
            std::stringstream ss;
            ss << op << " Bullet *1*\n"
               << "  Line continuation\n"
               << "\n"
               << "  extra paragraph\n"
               << op << " Bullet **2**";
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                "<ul>"
                    "<li>" + prefix + "<p>Bullet <em>1</em> Line continuation</p>"
                        "<p>extra paragraph</p>"
                    "</li>"
                    "<li>" + prefix + "<p>Bullet <strong>2</strong></p></li>"
                "</ul>"
            );
        }
        SECTION("Bullet list with code using " + d) {
            std::stringstream ss;
            ss << op << " Test\n"
               << "  ```cpp\n"
               << "  code content\n"
               << "  ```\n"
               << op << " Second point";
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                "<ul>"
                    "<li>" + prefix +
                    "<p>Test</p>"
                        "<pre><code class=\"language-cpp\">code content</code></pre>"
                    "</li>"
                    "<li>" + prefix +
                        "<p>Second point</p>"
                    "</li>"
                "</ul>"
            );
        }
        SECTION("Bullet list with code directly under bullet with " + d) {
            std::stringstream ss;
            ss << op << " ```cpp\n"
               << "  code content\n"
               << "  ```\n"
               << op << " Second point";
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                "<ul>"
                    "<li>" + prefix +
                    "<pre><code class=\"language-cpp\">code content</code></pre>"
                    "</li>"
                    "<li>" + prefix +
                    "<p>Second point</p>"
                    "</li>"
                "</ul>"
            );
        }
        SECTION("Bullet list with nested bullet lists should work" + d) {
            std::stringstream ss;
            ss << std::format(
                R"(
{0} List
  Continuation
{0} List
  Continuation
  {0} Sublist
    {0} Subsublist
)",
                op);

            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                std::format(
                    "<ul>"
                        "<li>{0}<p>List Continuation</p></li>"
                        "<li>{0}<p>List Continuation</p>"
                            "<ul><li>{0}<p>Sublist</p>"
                                "<ul><li>{0}<p>Subsublist</p></li></ul>"
                            "</li></ul>"
                        "</li>"
                    "</ul>",
                    prefix
                )
            );
        }
    };
    impl("*", 0);
    impl("-", 0);
    // Checklists should pass the same standard test suite
    impl("*", ' ');
    impl("-", ' ');
    impl("*", 'x');
    impl("-", 'x');
    impl("*", 'X');
    impl("-", 'X');
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
