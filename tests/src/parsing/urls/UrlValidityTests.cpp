#include <catch2/catch_test_macros.hpp>

#include "fixtures/TestSiteFixture.hpp"
#include "violet/parsing/LinkTranslate.hpp"

namespace {

TEST_CASE_PERSISTENT_FIXTURE(
    (tests::TestSiteFixture<tests::SiteVariant::TestMonolith, true>),
    "Internal URLs to files that exist should be recognized as valid"
) {
    std::unordered_map<std::string, std::string> testLinkPairs = {
        {"README.md", "listPagesPaginatedTests.md"},
        {"README.md", "/"},
        {"README.md", "/treePages"},
        {"README.md", "/treePages/folder"},
        {"README.md", "/treePages/folder/index.md"},
        {"listPagesPaginatedTests.md", "/"},
        {"listPagesPaginatedTests.md", "/json/load_data.md"},
        {"json_altroot/load_data.md", "/json/load_data.md"},
        {"json_altroot/load_data.md", "../json/load_data.md"},

    };

    for (auto& [k, v] : testLinkPairs) {
        INFO(k << " with link to " << v);
        REQUIRE(
            violet::checkPathValidity(
                k,
                v,
                sourcePath()
            )
        );
    }
}

TEST_CASE_PERSISTENT_FIXTURE(
    (tests::TestSiteFixture<tests::SiteVariant::TestMonolith, true>),
    "Internal URLs to files that don't exist should be recognized as invalid"
) {
    std::unordered_map<std::string, std::string> testLinkPairs = {
        {"README.md", "json/"},
        {"README.md", "/json_altroot/"},
        {"json_altroot/load_data.md", "/json_altroot/"},
        {"json_altroot/load_data.md", "../json_altroot/"},
        {"json_altroot/load_data.md", "json/load_data.md"},
        {"json_altroot/load_data.md", "__should_not_exist/whatever.md"},
        {"json_altroot/load_data.md", "/usr/bin/bash"},
    };

    for (auto& [k, v] : testLinkPairs) {
        INFO(k << " with link to " << v);
        REQUIRE_FALSE(
            violet::checkPathValidity(
                k,
                v,
                sourcePath()
            )
        );
    }
}

}
