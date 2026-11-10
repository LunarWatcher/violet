#include <catch2/catch_test_macros.hpp>

#include "fixtures/TestSiteFixture.hpp"

TEST_CASE_PERSISTENT_FIXTURE(
    tests::MonolithRealPrefix,
    "Verify that the alt monolith render location is correct"
) {
    REQUIRE(
        this->gen->getConfig().output()
        ==
        output()
    );
    REQUIRE(output().filename() == outputDir());
    auto expected = output() / "_violet-meta.json";
    INFO("Expected: " << expected.string());

    REQUIRE(std::filesystem::exists(expected));
}
