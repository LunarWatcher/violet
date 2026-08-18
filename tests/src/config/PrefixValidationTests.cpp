#include "violet/conf/Config.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Prefix /-ness should be validated") {
    violet::Config dummy;

    SECTION("/ must be required at the start of the string, but not the end") {
        REQUIRE_THROWS(dummy.setPrefix("test/test"));
        REQUIRE_THROWS(dummy.setPrefix("/test/"));
    }

    SECTION("Standard paths should not throw") {
        REQUIRE_NOTHROW(dummy.setPrefix("/test"));
        REQUIRE(dummy.prefix == "/test");

        REQUIRE_NOTHROW(dummy.setPrefix("/test/more"));
        REQUIRE(dummy.prefix == "/test/more");
    }

    SECTION("Empty prefixes are legal") {
        REQUIRE_NOTHROW(dummy.setPrefix(""));
        REQUIRE(dummy.prefix == "");
    }

    SECTION("/ prefixes should be equivalent to the empty prefix") {
        REQUIRE_NOTHROW(dummy.setPrefix("/"));
        REQUIRE(dummy.prefix == "");
    }
}
