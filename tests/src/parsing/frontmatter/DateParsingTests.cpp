#include <catch2/catch_test_macros.hpp>
#include "FrontmatterFixture.hpp"
#include "fixtures/WithTimezone.hpp"
#include "utils/Assertions.hpp"
#include <vector>
#include "violet/data/VioletGlobal.hpp"
#include "violet/parsing/DateTimeParsing.hpp"

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
        SECTION("UTC") {
            tests::WithTimezone tz("Etc/GMT");
            REQUIRE(
                REQUIRE_VALUE(
                    parse(R"({ "date": "2026-02-05T00:00:00" })").date
                )
                ==
                1770249600ll
            );
        }
        SECTION("Non-UTC") {
            tests::WithTimezone tz("Europe/Oslo");
            REQUIRE(
                REQUIRE_VALUE(
                    parse(R"({ "date": "2026-02-05T00:00:00" })").date
                )
                ==
                1770246000ll
            );
        }
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
        REQUIRE(
            REQUIRE_VALUE(
                parse(R"({ "date": "2026-02-05T00:00:00+0200" })").date
            )
            ==
            1770242400ll
        );
    }
}

TEST_CASE_METHOD(tests::FrontmatterFixture, "ISO-8601 dates in frontmatter should be parsed correctly") {
    tests::WithTimezone tz;
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

TEST_CASE_METHOD(tests::FrontmatterFixture, "ISO-8601 dates with a non-GMT timezone should be parsed in the correct timezone") {
    struct TestCaseData {
        std::string timezone;
        int64_t expected;
        int64_t expectedDifferenceHours;
    };
    auto utcBaseline = 1770249600ll;

    std::vector<TestCaseData> timezoneDateMap = {
        { "Etc/GMT", utcBaseline, 0 },
        { "Europe/Oslo", 1770246000ll, 1 },
        { "America/Toronto", 1770267600ll, -5 },
        { "Asia/Tokyo", 1770217200ll, 9 },
    };

    for (auto& [timezone, expected, expectedDiff] : timezoneDateMap) {
        INFO("Using timezone " << timezone);
        tests::WithTimezone tz(timezone);

        for (const auto& testDate : { "2026-02-05", "2026-02-05T00:00:00" }) {
            INFO("Using input date " << testDate);
            auto val = REQUIRE_VALUE(
                parse(
                    std::format(
                        R"({{ "date": "{}" }})", testDate
                    )
                ).date
            );
            REQUIRE((utcBaseline - val) == (expectedDiff * 60 * 60));
            auto formatted = violet::formatDate(
                val, violet::DateFormatISO, violet::Config::timezone
            );
            auto formattedUtc = violet::formatDate(
                val, violet::DateFormatISO, "Etc/GMT"
            );

            // we run this assertion first because it makes it easier to debug the next assertion
            REQUIRE(formatted == std::format(
                    // 0**3** because the sign counts towards padding, so 02 results in +1 instead of +01
                    "2026-02-05T00:00:00{:+03d}:00", expectedDiff
                ));
            // Double-check that the real offset in UTC != midnight UTC
            REQUIRE(formattedUtc == std::format(
                    "2026-02-0{}T{:02d}:00:00+00:00",
                    expectedDiff > 0 ? '4' : '5',
                    (24 - expectedDiff) % 24
                ));
            REQUIRE(val == expected);
        }
    }
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
