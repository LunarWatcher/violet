#include "violet/parsing/LinkTranslate.hpp"
#include <catch2/catch_test_macros.hpp>
#include <functional>

TEST_CASE("renameFile") {
    SECTION("Space words") {
        REQUIRE(violet::renameFile("Project structure.md") == "project-structure.md");
    }
}

TEST_CASE("projectBasedTranslator - no prefix, subpath") {

    std::string pathPrefix = "";
    std::filesystem::path projectRoot = "/tmp/violet-site";
    std::filesystem::path relSourceFilePath = "subpath/file.md";
    auto call = std::bind(
        violet::projectBasedTranslator,
        pathPrefix, projectRoot, relSourceFilePath, std::placeholders::_1
    );

    SECTION("relative URLs") {
        SECTION("Dumb") {
            REQUIRE(
                call("../../../../../../../../../file.md")
                ==
                "/file.html"
            );
            REQUIRE(
                call("/../../../../../../../../../file.md")
                ==
                "/file.html"
            );
        }
        SECTION("Malicious") {
            REQUIRE(
                call("../../../../../../../../etc/passwd")
                ==
                "/etc/passwd"
            );
            REQUIRE(
                call("/../../../../../../../../etc/passwd")
                ==
                "/etc/passwd"
            );
        }
    }

    SECTION("Misc rewrites") {
        REQUIRE(
            call("/../../../../../../../../../Trans rights are human rights.md")
            ==
            "/trans-rights-are-human-rights.html"
        );
        REQUIRE(
            call("/../../../../../../../../../README.md")
            ==
            "/index.html"
        );
        REQUIRE(
            call("README.md")
            ==
            "/subpath/index.html"
        );
        REQUIRE(
            call("/README.md")
            ==
            "/index.html"
        );
        REQUIRE(
            call("/My file.md")
            ==
            "/my-file.html"
        );
    }

    SECTION("Implicit index") {
        REQUIRE(
            call("/")
            ==
            "/"
        );
    }
}

TEST_CASE("projectBasedTranslator - prefix, subpath") {

    std::string pathPrefix = "/violet/";
    std::filesystem::path projectRoot = "/tmp/violet-site";
    std::filesystem::path relSourceFilePath = "subpath/file.md";
    auto call = std::bind(
        violet::projectBasedTranslator,
        pathPrefix, projectRoot, relSourceFilePath, std::placeholders::_1
    );

    SECTION("relative URLs") {
        SECTION("Dumb") {
            REQUIRE(
                call("../../../../../../../../../file.md")
                ==
                "/violet/file.html"
            );
            REQUIRE(
                call("/../../../../../../../../../file.md")
                ==
                "/violet/file.html"
            );
        }
        SECTION("Malicious") {
            REQUIRE(
                call("../../../../../../../../etc/passwd")
                ==
                "/violet/etc/passwd"
            );
            REQUIRE(
                call("/../../../../../../../../etc/passwd")
                ==
                "/violet/etc/passwd"
            );
        }
    }

    SECTION("Misc rewrites") {
        REQUIRE(
            call("/../../../../../../../../../Trans rights are human rights.md")
            ==
            "/violet/trans-rights-are-human-rights.html"
        );
        REQUIRE(
            call("/../../../../../../../../../README.md")
            ==
            "/violet/index.html"
        );
        REQUIRE(
            call("README.md")
            ==
            "/violet/subpath/index.html"
        );
        REQUIRE(
            call("/README.md")
            ==
            "/violet/index.html"
        );
        REQUIRE(
            call("/My file.md")
            ==
            "/violet/my-file.html"
        );
    }

    SECTION("Implicit index") {
        REQUIRE(
            call("/")
            ==
            "/violet/"
        );
    }
}

TEST_CASE("projectBasedTranslator - prefix (proper), subpath") {

    std::string pathPrefix = "/violet";
    std::filesystem::path projectRoot = "/tmp/violet-site";
    std::filesystem::path relSourceFilePath = "subpath/file.md";
    auto call = std::bind(
        violet::projectBasedTranslator,
        pathPrefix, projectRoot, relSourceFilePath, std::placeholders::_1
    );

    SECTION("relative URLs") {
        SECTION("Dumb") {
            REQUIRE(
                call("../../../../../../../../../file.md")
                ==
                "/violet/file.html"
            );
            REQUIRE(
                call("/../../../../../../../../../file.md")
                ==
                "/violet/file.html"
            );
        }
        SECTION("Malicious") {
            REQUIRE(
                call("../../../../../../../../etc/passwd")
                ==
                "/violet/etc/passwd"
            );
            REQUIRE(
                call("/../../../../../../../../etc/passwd")
                ==
                "/violet/etc/passwd"
            );
        }
        SECTION("Actual use-cases") {
            REQUIRE(
                call("file.webp")
                ==
                "/violet/subpath/file.webp"
            );
        }
    }

    SECTION("Misc rewrites") {
        REQUIRE(
            call("/../../../../../../../../../Trans rights are human rights.md")
            ==
            "/violet/trans-rights-are-human-rights.html"
        );
        REQUIRE(
            call("/../../../../../../../../../README.md")
            ==
            "/violet/index.html"
        );
        REQUIRE(
            call("README.md")
            ==
            "/violet/subpath/index.html"
        );
        REQUIRE(
            call("/README.md")
            ==
            "/violet/index.html"
        );
        REQUIRE(
            call("/My file.md")
            ==
            "/violet/my-file.html"
        );
    }

    SECTION("Implicit index") {
        REQUIRE(
            call("/")
            ==
            "/violet/"
        );
    }
}

TEST_CASE("projectBasedTranslator should handle relative files in subpaths") {
    std::string pathPrefix = "/violet";
    std::filesystem::path projectRoot = "/tmp/violet-site";
    std::filesystem::path relSourceFilePath = "subpath/subsubpath/subsubsubpath/file.md";
    auto call = std::bind(
        violet::projectBasedTranslator,
        pathPrefix, projectRoot, relSourceFilePath, std::placeholders::_1
    );

    SECTION("Relative paths should be handled correctly") {
        REQUIRE(
            call("file.txt") == "/violet/subpath/subsubpath/subsubsubpath/file.txt"
        );
        REQUIRE(
            call("../file.txt") == "/violet/subpath/subsubpath/file.txt"
        );
        REQUIRE(
            call("../../file.txt") == "/violet/subpath/file.txt"
        );
        REQUIRE(
            call("../../../file.txt") == "/violet/file.txt"
        );
        REQUIRE(
            call("../../../../file.txt") == "/violet/file.txt"
        );
        REQUIRE(
            call("path/file.txt") == "/violet/subpath/subsubpath/subsubsubpath/path/file.txt"
        );
        REQUIRE(
            call("deeper/path/file.txt") == "/violet/subpath/subsubpath/subsubsubpath/deeper/path/file.txt"
        );
    }
}
