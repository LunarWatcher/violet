#include <catch2/catch_test_macros.hpp>

#include "fixtures/TestSiteFixture.hpp"

TEST_CASE_PERSISTENT_FIXTURE(
    tests::MonolithRealPrefix,
    "Verify abspath on root node (real prefix)"
) {
    auto data = getDebug("abspath.html");

    std::unordered_map<std::string, std::string> expected = {
        {"abspath-1", "/prefix"},
        {"abspath-transform", "/prefix/treePages/index.html"},
        {"abspath-notransform", "/prefix/treePages/README.md"},
        {"abspath-escape", "/prefix/etc/passwd"},
        {"abspath-rel", "/prefix/abspath.html"},
        {"abspath-rel-alt", "/prefix/abspath.html"},
    };

    REQUIRE(data.size() == expected.size());

    for (auto& [key, expectedValue] : expected) {
        INFO("Test key: " << key);
        REQUIRE(data.contains(key));
        REQUIRE(data.at(key) == expectedValue);
    }
}

TEST_CASE_PERSISTENT_FIXTURE(
    tests::MonolithRealPrefix,
    "Verify abspath on nested node (real prefix)"
) {
    auto data = getDebug("misc/nested/abspath.html");

    std::unordered_map<std::string, std::string> expected = {
        {"abspath-1", "/prefix"},
        {"relpath", "/prefix/misc/nested"},
        {"abspath-transform", "/prefix/treePages/index.html"},
        {"abspath-notransform", "/prefix/treePages/README.md"},
        {"abspath-escape", "/prefix/etc/passwd"},
        {"abspath-rel", "/prefix/misc/nested/abspath.html"},
        {"abspath-rel-alt", "/prefix/misc/nested/abspath.html"},
    };

    REQUIRE(data.size() == expected.size());

    for (auto& [key, expectedValue] : expected) {
        INFO("Test key: " << key);
        REQUIRE(data.contains(key));
        REQUIRE(data.at(key) == expectedValue);
    }
}
