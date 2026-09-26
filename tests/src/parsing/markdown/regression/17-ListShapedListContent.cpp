#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

#include "parsing/markdown/fixture/MarkdownFixture.hpp"

TEST_CASE_METHOD(tests::MarkdownFixture, "List-shaped content under a list should not cause problems", "[markdown][lists][regression]") {
    std::stringstream ss;
    SECTION("Number under bullet") {
        auto impl = [&](const std::string& bulletChar, const std::string& extras) {
            SECTION("With bulletChar = " + bulletChar) {
                ss << std::format(R"({0} 0.10.0 content
{0} - ul
{0} - [x]

[x]: https://example.com)", bulletChar);
                REQUIRE(
                    violet::Markdown::parse(ss)
                    ==
                    std::format(
                        "<ul>"
                            "<li>{0}<p>0.10.0 content</p></li>"
                            "<li>{0}<p>- ul</p></li>"
                            R"(<li>{0}<p>- <a href="https://example.com">x</a></p></li>)"
                        "</ul>",
                        extras
                    )
                );
            }
        };

        impl("*", "");
        impl("-", "");
        impl("- [ ]", checklist(false));
        impl("- [x]", checklist(true));
        impl("- [X]", checklist(true));
        
    }

    SECTION("Bullet under number") {
        ss << R"(1. - bullet
2. \* bullet
3. - [x] bullet

[x]: https://example.com)";

        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ol>"
                "<li><p>- bullet</p></li>"
                "<li><p>* bullet</p></li>"
                R"(<li><p>- <a href="https://example.com">x</a> bullet</p></li>)"
            "</ol>"
        );
    }
}
