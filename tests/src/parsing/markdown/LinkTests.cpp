#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <utils/LinkableFeatureParityTestCaseGenerator.hpp>

namespace {

TEST_CASE_METHOD(tests::MarkdownFixture, "Plain link references should work for links") {
    REQUIRE(
        render(
            R"([link-ref]

[link-ref]: https://example.com)"
        )
        ==
        paragraph({
                url("link-ref", "https://example.com")
            })
    );
}

}
