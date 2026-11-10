#include "violet/parsing/LinkTranslate.hpp"
#include <catch2/catch_test_macros.hpp>
#include <functional>

namespace {

std::function<std::string(const std::string&)> genCall(
    const std::string& pathPrefix,
    const std::filesystem::path& relSourceFile
) {
    return std::bind(
        violet::projectBasedTranslator,
        pathPrefix, relSourceFile, std::nullopt, std::placeholders::_1
    );
}

}

TEST_CASE("renameFile") {
    SECTION("Space words") {
        REQUIRE(violet::renameFile("Project structure.md") == "project-structure.html");
    }
}

// TODO: These tests could be generated sections with a vector
TEST_CASE("projectBasedTranslator - no prefix, subpath") {
    auto call = genCall("", "subpath/file.md");

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
    auto call = genCall("/violet/", "subpath/file.md");

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
            "/violet"
        );
    }
}

TEST_CASE("projectBasedTranslator - prefix (proper), subpath") {
    auto call = genCall("/violet", "subpath/file.md");

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
            "/violet"
        );
    }
}

TEST_CASE("projectBasedTranslator should handle relative files in subpaths") {
    auto call = genCall("/violet", "subpath/subsubpath/subsubsubpath/file.md");

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

#define REQUIRE_CALL_EQUALS(string) REQUIRE(call(string) == string);

TEST_CASE("projectBasedTranslator should return HTTP links as-is") {
    auto call = genCall("/violet", "subpath/subsubpath/subsubsubpath/file.md");

    REQUIRE_CALL_EQUALS("https://example.com");
    REQUIRE_CALL_EQUALS("http://example.com");
    REQUIRE_CALL_EQUALS("https:");
    REQUIRE_CALL_EQUALS("http:");
    REQUIRE_CALL_EQUALS("https://academictorrents.com/details/82075af82e83b99e9a4c6ef17e564947efe65a66");
    REQUIRE_CALL_EQUALS("https://en.wikipedia.org/wiki/Wikipedia:Database_download");
    REQUIRE_CALL_EQUALS("https://codeberg.org/explore/repos?q=violet-ssg&topic=1");
    REQUIRE_CALL_EQUALS("");
}

TEST_CASE("projectBasedTranslator should return non-HTTP non-site-relative URIs as-is") {
    auto call = genCall("/violet", "subpath/subsubpath/subsubsubpath/file.md");

    // Test cases based on examples shown in https://www.rfc-editor.org/info/rfc3986/. stc uses these as well to verify
    // the URI parser, so we might as well verify that they're all treated as external URLs
    REQUIRE_CALL_EQUALS("ftp://ftp.is.co.za/rfc/rfc1808.txt");
    REQUIRE_CALL_EQUALS("ldap://[2001:db8::7]/c=GB?objectClass?one");
    REQUIRE_CALL_EQUALS("mailto:John.Doe@example.com");
    REQUIRE_CALL_EQUALS("news:comp.infosystems.www.servers.unix");
    REQUIRE_CALL_EQUALS("tel:+1-816-555-1212");
    REQUIRE_CALL_EQUALS("telnet://192.0.2.16:80/");
    REQUIRE_CALL_EQUALS("urn:oasis:names:specification:docbook:dtd:xml:4.1.2");

    // Magnet URL for https://academictorrents.com/details/82075af82e83b99e9a4c6ef17e564947efe65a66
    REQUIRE_CALL_EQUALS("magnet:?xt=urn:btih:82075af82e83b99e9a4c6ef17e564947efe65a66&tr=https%3A%2F%2Facademictorrents.com%2Fannounce.php&tr=udp%3A%2F%2Ftracker.coppersurfer.tk%3A6969&tr=udp%3A%2F%2Ftracker.opentrackr.org%3A1337%2Fannounce");
}

TEST_CASE("projectBasedTranslator should handle different fragments and queries") {
    auto call = genCall("/violet", "subpath/subsubpath/subsubsubpath/file.md");

    REQUIRE_CALL_EQUALS(
        "#fragment"
    );
    REQUIRE_CALL_EQUALS(
        "?trans-rights=human-rights#fragment"
    );
    REQUIRE(
        call("./#fragment") == "/violet/subpath/subsubpath/subsubsubpath#fragment"
    );
    REQUIRE(
        call("..#fragment") == "/violet/subpath/subsubpath#fragment"
    );
    REQUIRE(
        call("/../#fragment") == "/violet#fragment"
    );
}

TEST_CASE("projectBasedTranslator should handle quotes in the string") {
    auto call = genCall("/violet", "subpath/subsubpath/subsubsubpath/file.md");

    REQUIRE(
        call("\"?query=\"test\"#frag\"ment") == "/violet/subpath/subsubpath/subsubsubpath/%22?query=%22test%22#frag%22ment"
    );
    REQUIRE(
        call("/subpath/subsubpath/subsubsubpath/%22?query=%22test%22#frag%22ment")
        ==
        "/violet/subpath/subsubpath/subsubsubpath/%22?query=%22test%22#frag%22ment"
    );
}
