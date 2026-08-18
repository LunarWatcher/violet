#include <catch2/catch_test_macros.hpp>

#include <violet/parsing/Escaping.hpp>

#define DefineEscaping(content) [](bool includeReserved, bool includeSlash) { \
        std::string in = content;                                       \
        std::stringstream out;                                          \
        violet::urlEncode(in, out, includeReserved, includeSlash);      \
        return out.str();                                               \
    }
    

namespace {

TEST_CASE("URL encoding should work") {
    auto dotSlash = DefineEscaping("./#fragment");
    SECTION("Reserved included") {
        REQUIRE(
            dotSlash(true, true) == ".%2f%23fragment"
        );
        REQUIRE(
            dotSlash(true, false) == "./%23fragment"
        );
        REQUIRE(
            dotSlash(false, true) == ".%2f#fragment"
        );
        REQUIRE(
            dotSlash(false, false) == "./#fragment"
        );
    }
}

}
