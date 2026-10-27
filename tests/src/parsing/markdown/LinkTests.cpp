#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "parsing/markdown/fixture/MarkdownFixture.hpp"

namespace {

TEST_CASE_METHOD(tests::MarkdownFixture, "Parentheses in links should be handled") {
    SECTION("Balanced") {
        REQUIRE(
            render(
                "[link](https://en.wikipedia.org/wiki/Violet_(color))"
            )
            ==
            paragraph(url("link", "https://en.wikipedia.org/wiki/Violet_(color)"))
        );
        REQUIRE(
            render(
                "[link](https://en.wikipedia.org/wiki/Violet_(color)))"
            )
            ==
            paragraph(url("link", "https://en.wikipedia.org/wiki/Violet_(color)") + ")")
        );
        REQUIRE(
            render(
                "[link](https://en.wikipedia.org/wiki/Violet_(color)) with text)"
            )
            ==
            paragraph(url("link", "https://en.wikipedia.org/wiki/Violet_(color)") + " with text)")
        );
    }
    SECTION("Unbalanced") {
        std::string err =
            "Unclosed URL. Hint: if your URL contains unbalanced parentheses and it's meant to "
            "be unbalanced, use an escaped link (`[text](<https://en.wikipedia.org/wiki/Violet_(color>)`). Otherwise, make "
            "sure you actually closed this URL correctly.";
        REQUIRE_THROWS_WITH(
            render(
                "[link](https://en.wikipedia.org/wiki/Violet_(color)"
            ),
            err
        );
        REQUIRE_THROWS_WITH(
            render(
                "[link](https://en.wikipedia.org/wiki/Violet_(color) with text"
            ),
            err
        );
        REQUIRE_THROWS_WITH(
            render(
                "[link](https://en.wikipedia.org/wiki/Violet_(color)with text"
            ),
            err
        );
    }
    SECTION("Unbalanced with trailing text that contains a parenthesis") {
        REQUIRE(
            render(
                "[link](https://en.wikipedia.org/wiki/Violet_(color)with text)"
            )
            ==
            paragraph(
                url("link", "https://en.wikipedia.org/wiki/Violet_(color)with text")
            )
        );
    }
}

TEST_CASE_METHOD(tests::MarkdownFixture, "Literal URLs") {
    REQUIRE(
        render("[link](<https://en.wikipedia.org/wiki/Violet_(color>)")
        ==
        paragraph(
            url("link", "https://en.wikipedia.org/wiki/Violet_(color")
        )
    );
    REQUIRE_THROWS(
        render("[link](<https://en.wikipedia.org/wiki/Violet_(color>")
    );
    REQUIRE_THROWS(
        render("[link](<https://en.wikipedia.org/wiki/Violet_(color))")
    );
}

}
