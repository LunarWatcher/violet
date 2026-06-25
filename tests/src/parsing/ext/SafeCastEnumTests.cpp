#include "violet/generate/templates/ext/SafeCastEnum.hpp"
#include <catch2/catch_test_macros.hpp>

#define IntVal(enumVal) static_cast<int>(enumVal)

namespace {

enum class TestCase {
    Start,
    A,
    B,
    C,
    D,
    End
};

}

TEST_CASE("Test safeCastEnum validity") {
    SECTION("Boundries should be invalid") {
        REQUIRE(
            violet::safeCastEnum<TestCase>(IntVal(TestCase::Start)) == std::nullopt
        );
        REQUIRE(
            violet::safeCastEnum<TestCase>(IntVal(TestCase::Start)) == std::nullopt
        );
    }
    SECTION("Legal values should return legal values") {
        std::vector<TestCase> values {
            TestCase::A,
            TestCase::B,
            TestCase::C,
            TestCase::D
        };
        for (auto value : values) {
            REQUIRE(
                violet::safeCastEnum<TestCase>(IntVal(value)).has_value()
            );
            REQUIRE(
                violet::safeCastEnum<TestCase>(IntVal(value)) == value
            );
        }
    }
    SECTION("Illegal values should be invalid") {
        REQUIRE_FALSE(
            violet::safeCastEnum<TestCase>(7000).has_value()
        );
        REQUIRE_FALSE(
            violet::safeCastEnum<TestCase>(-7000).has_value()
        );
        REQUIRE_FALSE(
            violet::safeCastEnum<TestCase>(0).has_value() // manual boundry (Start)
        );
        REQUIRE_FALSE(
            violet::safeCastEnum<TestCase>(5).has_value() // manual boundry (End)
        );
        REQUIRE_FALSE(
            violet::safeCastEnum<TestCase>(1000000).has_value() // manual boundry (End)
        );
    }
}
