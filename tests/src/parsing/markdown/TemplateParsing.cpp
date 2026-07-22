#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Templates should be left as-is") {
    SECTION("on paragraph level") {
        std::stringstream ss;
        ss << R"(This is a line
{{ page.table_of_contents }}
This is another line
)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<p>This is a line</p>\n"
            "{{ page.table_of_contents }}\n"
            "<p>This is another line</p>"
        );
    }

    SECTION("In list") {
        std::stringstream ss;
        ss << R"(1. This is a list
  {{ page.table_of_contents }}
2. This is also a list
)";

        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ol>"
                "<li><p>This is a list</p>\n"
                "{{ page.table_of_contents }}\n"
                "</li>"
                "<li><p>This is also a list</p></li>"
            "</ol>"
        );
    }
 
    SECTION("At start of list") {
        std::stringstream ss;
        ss << R"(1. {{ page.table_of_contents }}
2. This is also a list
)";

        // TODO: maybe a line starting with a space (after indent) should mean raw mode?
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ol>"
                "<li>\n"
                "{{ page.table_of_contents }}\n"
                "</li>"
                "<li><p>This is also a list</p></li>"
            "</ol>"
        );
    }
    SECTION("Nested blocks in list") {
        std::stringstream ss;
        ss << R"(1. {% block whatever %}
    {% for whatever in listPages(site, ".") %}
      owo
    {% endfor %}
  {% end %}
2. This is also a list
)";

        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ol>"
                "<li>\n"
                "{% block whatever %}\n"
                    "  {% for whatever in listPages(site, \".\") %}\n"
                    "    owo\n"
                    "  {% endfor %}\n"
                    "{% end %}\n"
                "</li>"
                "<li><p>This is also a list</p></li>"
            "</ol>"
        );
    }
}


TEST_CASE("HTML should be left alone") {
    SECTION("Basic test case") {
        std::stringstream ss;
        ss << R"(<div>
  <img src="https://example.com" />
</div>)";
        // TODO: do we actually want to keep the newline? I don't know if it actually makes an impact. It probably made
        // more sense when it was just templates, but I'm not sure if the newline breaks anything if it sort of ends up
        // inline.
        std::string expected = "\n" + ss.str();
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            expected
        );
    }
}
