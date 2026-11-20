#include <catch2/catch_test_macros.hpp>

#include "fixtures/TestSiteFixture.hpp"
#include "violet/data/git/GitVersion.hpp"

TEST_CASE_PERSISTENT_FIXTURE(
    tests::MonolithRealPrefix,
    "Verify that assets with templates in themes are parsed as templates"
) {
    auto p = getBuildFile("assets/_default_page.js");
    INFO("Trying to load " << p.string());
    std::ifstream f(p);

    REQUIRE(bool(f));

    std::stringstream content;
    content << f.rdbuf();
    INFO(content.str());
    REQUIRE_FALSE(
        content.str().starts_with("---")
    );
    // Both the CSS and JS file contain a `violet <version>` bit. We can compare this to the GitVersion to see if it was
    // parsed. If template parsing isn't happening, we expect this to fail
    REQUIRE(
        content.str().contains("violet " + std::string { violet::GitVersion() })
    );
}
