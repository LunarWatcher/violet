#include <catch2/catch_test_macros.hpp>

#include "fixtures/TestSiteFixture.hpp"

TEST_CASE_PERSISTENT_FIXTURE(
    (tests::TestSiteFixture<tests::SiteVariant::TestMonolith, true>),
    "Verify that listPages on monolith/treePages doesn't expose README.md when index.md is present"
) {
    SECTION("Verify raw listPages output") {
        auto data = getDebug("treePages/index.html");
        REQUIRE(data.contains("listPages"));

        // I do not like having to hard-code a string this long here, but it work shaped
        REQUIRE(
            data.at("listPages")
            ==
            R"([{"data":{},"taxonomies":{},"violet_internals.path":"treePages/README.md","table_of_contents":"","hidden":false,"layout":"single_page","type":"_default","violet_internals.type":2,"url":"treePages/index.html","title":"Root"},{"data":{},"taxonomies":{},"violet_internals.path":"treePages/folder/index.md","table_of_contents":"","hidden":false,"layout":"single_page","type":"_default","violet_internals.type":2,"url":"treePages/folder/index.html","title":"index"}])"
        );
    }
}
