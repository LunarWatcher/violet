#include <filesystem>
#include <fixtures/LoadWorkspace.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("InitWorkspace should change the directory") {
    auto cwd = std::filesystem::current_path();

    {
        tests::LoadWorkspace ws{
            tests::BaseTestSite
        };
        REQUIRE(std::filesystem::current_path() != cwd);
        REQUIRE(std::filesystem::exists("violet.json"));
    }

    REQUIRE(std::filesystem::current_path() == cwd);
    REQUIRE_FALSE(std::filesystem::exists("violet.json"));
}
