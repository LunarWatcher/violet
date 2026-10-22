#include <catch2/catch_test_macros.hpp>
#include "FrontmatterFixture.hpp"
#include "utils/Assertions.hpp"

namespace {

// Note: all the tests in this file rely on magic numbers to avoid the function we're testing being used to validate the
// function of the function we're testing.
//
// The numbers are generated with
// ```
// date -u -d "$full_timestamp" +%s
// ```
// against the full expected UTC timestamp, since that's what violet uses internally.

TEST_CASE_METHOD(tests::FrontmatterFixture, "ISO-8601 datetimes in frontmatter should be parsed correctly") {
    SECTION("Without timezones") {
        REQUIRE(
            REQUIRE_VALUE(
                parse(R"({ "date": "2026-02-05T00:00:00" })").date
            )
            ==
            // UTC should be assumed
            1770249600ll
        );
    }
    SECTION("With timezones") {
        REQUIRE(
            REQUIRE_VALUE(
                parse(R"({ "date": "2026-02-05T00:00:00Z" })").date
            )
            ==
            1770249600ll
        );
        REQUIRE(
            REQUIRE_VALUE(
                parse(R"({ "date": "2026-02-05T00:00:00+02:00" })").date
            )
            ==
            1770242400ll
        );
    }
}

TEST_CASE_METHOD(tests::FrontmatterFixture, "ISO-8601 dates in frontmatter should be parsed correctly") {
    REQUIRE(
        REQUIRE_VALUE(
            parse(R"({ "date": "2026-02-05" })").date
        )
        ==
        1770249600ll
    );
    REQUIRE(
        REQUIRE_VALUE(
            parse(R"({ "date": "1970-01-01" })").date
        )
        ==
        0ll
    );
}

TEST_CASE_METHOD(tests::FrontmatterFixture, "Non-ISO datetimes in frontmatter should throw") {
    REQUIRE_THROWS(
        parse(R"({ "date": "19:21:47" })")
    );
    REQUIRE_THROWS(
        parse(R"({ "date": "19:21:47Z" })")
    );
    REQUIRE_THROWS(
        parse(R"({ "date": "19:21:47+02:00" })")
    );
    REQUIRE_THROWS(
        parse(R"({ "date": "2026-09-09+02:00" })")
    );
    REQUIRE_THROWS(
        parse(R"({ "date": "1234567890" })")
    );
}

}
