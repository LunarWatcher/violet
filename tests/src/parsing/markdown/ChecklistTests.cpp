#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Markdown checklists", "[markdown][checklists]") {
    std::stringstream ss;
    SECTION("All checklists") {
        ss << R"(- [ ] Item
- [x] Item
- [ ] Item
)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ul>"
                R"(<li><input type="checkbox" disabled/><p>Item</p></li>)"
                R"(<li><input type="checkbox" disabled checked/><p>Item</p></li>)"
                R"(<li><input type="checkbox" disabled/><p>Item</p></li>)"
            "</ul>"
        );
    }
    SECTION("Mixed use lists are allowed") {
        ss << R"(- [ ] Item
- Item
- [x] Item
)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ul>"
                R"(<li><input type="checkbox" disabled/><p>Item</p></li>)"
                R"(<li><p>Item</p></li>)"
                R"(<li><input type="checkbox" disabled checked/><p>Item</p></li>)"
            "</ul>"
        );
    }
    SECTION("Should be allowed in nested lists") {
        ss << R"(- [ ] Item
  - [x] Item
)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ul>"
                R"(<li><input type="checkbox" disabled/><p>Item</p>)"
                    "<ul>"
                        R"(<li><input type="checkbox" disabled checked/><p>Item</p></li>)"
                    "</ul>"
                "</li>"
            "</ul>"
        );
    }
}
