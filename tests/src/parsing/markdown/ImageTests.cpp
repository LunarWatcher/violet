#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Images should work") {
    SECTION("Standalone images") {
        std::stringstream ss;
        ss << "![alt](https://example.com)";

        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><img src="https://example.com" alt="alt" /></p>)"
        );
    }
    SECTION("Standalone images with reference") {
        std::stringstream ss;
        ss << R"(![alt][1]

[1]: https://example.com)";

        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><img src="https://example.com" alt="alt" /></p>)"
        );
    }

    SECTION("Standalone images with reference in URL with direct URL") {
        std::stringstream ss;
        ss << R"([![alt][1]](https://kagi.com)

[1]: https://example.com)";

        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p><a href="https://kagi.com"><img src="https://example.com" alt="alt" /></a></p>)"
        );
    }
}
