#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>
#include <map>

TEST_CASE("Inline code should not consume duplicated characters", "[markdown][inline-code][regression]") {
    std::stringstream ss;
    SECTION("- should not be consumed") {
        ss << "`-f`";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<p><code>-f</code></p>"
        );
    }
    SECTION("-- should not be consumed") {
        ss << "`--root`";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<p><code>--root</code></p>"
        );
    }
    SECTION("- should not be consumed in an unordered list") {
        ss << "- `-f`";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ul><li><p><code>-f</code></p></li></ul>"
        );
    }
    SECTION("-- should not be consumed in an unordered list") {
        ss << "* `--root`";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ul><li><p><code>--root</code></p></li></ul>"
        );
    }
    SECTION("- should not be consumed in an ordered list") {
        ss << "1. `-f`";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ol><li><p><code>-f</code></p></li></ol>"
        );
    }
    SECTION("-- should not be consumed in an ordered list") {
        ss << "1. `--root`";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ol><li><p><code>--root</code></p></li></ol>"
        );
    }

    SECTION("The patch should not fuck inline code on a newline") {
        std::map<std::string, std::string> listRenderMap {
            {"*", "ul"},
            {"-", "ul"},
            {"1.", "ol"},
        };

        for (auto& [ch, expected] : listRenderMap) {
            std::stringstream ss;
            ss << std::format(
                R"({} This is a list
  `- with inline code` in continuation)",
                ch
            );
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                std::format(
                    "<{0}><li><p>This is a list <code>- with inline code</code> in continuation</p></li></{0}>",
                    expected
                )
            );
        }
    }

    SECTION("Violet's changelog should render properly") {
        std::map<std::string, std::string> cases = {
            {
                "* CLI (generate): Added `--root`, largely for debug and E2E test purposes",
                "CLI (generate): Added <code>--root</code>, largely for debug and E2E test purposes"
            },
            {
                "* Fixed a bug in list parsing that caused `* 0.10.0` to trigger an infinite parsing loop ([#17](https://codeberg.org/LunarWatcher/violet/issues/17))",
                R"(Fixed a bug in list parsing that caused <code>* 0.10.0</code> to trigger an infinite parsing loop (<a href="https://codeberg.org/LunarWatcher/violet/issues/17">#17</a>))"
            },
            {
                "* CLI (init): Added the `init` command for quickly setting up a template repo",
                "CLI (init): Added the <code>init</code> command for quickly setting up a template repo"
            },
            {
                "* 0.10.0's addition of `--prefix` broke prefixes due to a CLI12 oddity",
                "0.10.0's addition of <code>--prefix</code> broke prefixes due to a CLI12 oddity"
            },
        };

        for (auto& [source, expected] : cases) {
            std::stringstream ss;
            ss << source;
            REQUIRE(
                violet::Markdown::parse(ss)
                ==
                std::format(
                    "<ul><li><p>{}</p></li></ul>",
                    expected
                )
            );
        }
    }
}
