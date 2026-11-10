#include <catch2/catch_test_macros.hpp>

#include <violet/path/path.hpp>
#include <utils/Output.hpp>

namespace {

TEST_CASE("Path parsing") {
    REQUIRE(
        violet::path("test/path").getSegments()
        ==
        std::vector<std::string> { "test", "path" }
    );
    REQUIRE(
        violet::path("/test/path").getSegments()
        ==
        std::vector<std::string> { "test", "path" }
    );
    REQUIRE(
        violet::path("/a/b").getSegments()
        ==
        std::vector<std::string> { "a", "b" }
    );
    REQUIRE(
        violet::path("test/path/").getSegments()
        ==
        std::vector<std::string> { "test", "path" }
    );
    REQUIRE(
        violet::path("/test/path/").getSegments()
        ==
        std::vector<std::string> { "test", "path" }
    );
    REQUIRE(
        violet::path("///test/////////path////").getSegments()
        ==
        std::vector<std::string> { "test", "path" }
    );
#ifdef _WIN32
    SECTION("Same tests with the \\ separator") {
        REQUIRE(
            violet::path("test\\path").getSegments()
            ==
            std::vector<std::string> { "test", "path" }
        );
        REQUIRE(
            violet::path("\\test\\path").getSegments()
            ==
            std::vector<std::string> { "test", "path" }
        );
        REQUIRE(
            violet::path("test\\path\\").getSegments()
            ==
            std::vector<std::string> { "test", "path" }
        );
        REQUIRE(
            violet::path("\\test\\path\\").getSegments()
            ==
            std::vector<std::string> { "test", "path" }
        );
        REQUIRE(
            violet::path("\\\\\\test\\\\\\\\\\\\\\\\\\path\\\\\\\\").getSegments()
            ==
            std::vector<std::string> { "test", "path" }
        );
    }
#endif
}

TEST_CASE("Path traversal") {
    REQUIRE(
        violet::path("test/./path").getSegments()
        ==
        std::vector<std::string> { "test", "path" }
    );
    REQUIRE(
        violet::path("test/././path").getSegments()
        ==
        std::vector<std::string> { "test", "path" }
    );
    REQUIRE(
        violet::path("test/../path").getSegments()
        ==
        std::vector<std::string> { "path" }
    );
    REQUIRE(
        violet::path("../test/path").getSegments()
        ==
        std::vector<std::string> { "test", "path" }
    );
    REQUIRE(
        violet::path("../test/path/..").getSegments()
        ==
        std::vector<std::string> { "test" }
    );
    REQUIRE(
        violet::path("../../../").getSegments()
        == // equality with an empty vector is easier to debug than `.empty()`
        std::vector<std::string> {}
    );
}

TEST_CASE("Parent paths") {
    violet::path p("test/path/file.md");
    REQUIRE(std::string(p) == "/test/path/file.md");

    auto parent = p.parent_path();
    REQUIRE(parent.getSegments() == std::vector<std::string> { "test", "path" });
    parent = parent.parent_path();
    REQUIRE(parent.getSegments() == std::vector<std::string> { "test" });
    for (size_t i = 0; i < 10; ++i) {
        parent = parent.parent_path();
        REQUIRE(parent.getSegments() == std::vector<std::string> {});
    }
}

TEST_CASE("Path concat") {
    violet::path p("test/path/file.md");
    REQUIRE(
        p.concatPrefix("/my/prefix") == "/my/prefix/test/path/file.md"
    );
}

TEST_CASE("Path joining") {
    violet::path p("/usr/bin");
    p.join(std::string("bash"));
    REQUIRE(p.getSegments() == std::vector<std::string> { "usr", "bin", "bash" });
}

TEST_CASE("Path edge-cases", "[path]") {
    REQUIRE(
        violet::path("").getSegments()
        ==
        std::vector<std::string> {}
    );
    REQUIRE(
        violet::path("/").getSegments()
        ==
        std::vector<std::string> {}
    );
    REQUIRE(
        violet::path("/////////////////").getSegments()
        ==
        std::vector<std::string> {}
    );
}

}
