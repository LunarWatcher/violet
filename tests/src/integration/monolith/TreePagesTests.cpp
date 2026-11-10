#include <catch2/catch_test_macros.hpp>

#include "fixtures/TestSiteFixture.hpp"

TEST_CASE_PERSISTENT_FIXTURE(
    (tests::TestSiteFixture<tests::SiteVariant::TestMonolith, true>),
    "Verify that TreePages doesn't expose README.md when index.md is present"
) {
    SECTION("Verify raw treePages output") {
        auto data = getDebug("treePages/index.html");
        REQUIRE(data.contains("treePages"));

        // I do not like having to hard-code a string this long here, but it work shaped
        REQUIRE(
            data.at("treePages")
            ==
            R"({"index":{"data":{},"taxonomies":{},"violet_internals.path":"/treePages/README.md","table_of_contents":"","hidden":false,"layout":"single_page","type":"_default","violet_internals.type":2,"url":"/treePages/index.html","title":"Root"},"subfolders":[{"index":{"data":{},"taxonomies":{},"violet_internals.path":"/treePages/folder/index.md","table_of_contents":"","hidden":false,"layout":"single_page","type":"_default","violet_internals.type":2,"url":"/treePages/folder/index.html","title":"index"},"subfolders":[],"items":[],"folder_name":"folder"}],"items":[],"folder_name":"treePages"})"
        );
    }
    SECTION("Verify rendered treePages/folder/index.html output") {
        // We expect this to be the index
        auto data = getDebug("treePages/folder/index.html");
        REQUIRE(data.contains("page"));
        REQUIRE(data.at("page") == "index");
    }
}
