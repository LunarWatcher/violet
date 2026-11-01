#include <catch2/catch_test_macros.hpp>

#include <nlohmann/json.hpp>

#include "fixtures/TestSiteFixture.hpp"

TEST_CASE_PERSISTENT_FIXTURE(
    (tests::TestSiteFixture<tests::SiteVariant::TestMonolith, true>),
    "Test listPagesPaginated's sort methods"
) {
    auto debug = getDebug("listpagespaginatedtests.html");
    REQUIRE(debug.size() == 3);

    std::unordered_map<std::string, int64_t> created {
        {"1", 1790978400ll},
        {"2", 1791151200ll},
        {"3", 1790805600ll},
        {"4", 1790892000ll},
        {"5", 1791064800ll},
    };
    std::unordered_map<std::string, int64_t> lastModified {
        {"1", 1790892000ll},
        {"2", 1790978400ll},
        {"3", 1791151200ll},
        {"4", 1791064800ll},
        {"5", 1790805600ll},
    };

    auto impl = [&](const auto& key, const std::vector<std::string>& titleOrder) {
        REQUIRE(debug.contains(key));
        std::stringstream ss;
        ss << debug.at(key);
        nlohmann::json result;
        ss >> result;
        
        REQUIRE(result.is_array());
        REQUIRE(result.size() == 5);
        {
            INFO("titleOrder.size() != 5 only if there's a dev error");
            REQUIRE(titleOrder.size() == 5);
        }

        for (size_t i = 0; i < 5; ++i) {
            INFO("Index = " << i);
            INFO(result.at(i).dump(4));
            REQUIRE(result.at(i).at("date").get<int64_t>() == created.at(titleOrder.at(i)));
            REQUIRE(result.at(i).at("last_modified").get<int64_t>() == lastModified.at(titleOrder.at(i)));
            REQUIRE(result.at(i).at("title") == titleOrder.at(i));
        }
    };

    SECTION("byCreated") {
        impl("byCreated", {
            "2",
            "5",
            "1",
            "4",
            "3",
        });
    }
    SECTION("byLastModified") {
        impl("byModified", {
                "3",
            "4",
            "2",
            "1",
            "5",
        });
    }
    SECTION("byTitle") {
        impl("byTitle", {"1", "2", "3", "4", "5"});
    }
}
