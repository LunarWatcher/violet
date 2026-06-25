#include "fixtures/LoadWorkspace.hpp"
#include "fixtures/TestSiteGenerator.hpp"
#include "utils/FileReader.hpp"
#include "utils/StringUtils.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Blog integration tests") {
    tests::TestSiteGenerator testGen {
        tests::SiteVariant::BlogTestSite
    };
    testGen.generateWithAsserts();

    SECTION("The Atom feed should exist and be generated from feed.inja") {
        std::ifstream f(testGen.buildPath() / "blog/feed.atom");
        INFO("Trying to open blog/feed.atom");
        REQUIRE(bool(f));

        std::string content = tests::readFile(f);
        INFO("Content: " << content);
        {
            INFO("The file must start with the XML header thing");
            REQUIRE(content.starts_with(R"(<?xml version="1.0" encoding="utf-8"?>)"));
        }
        {
            INFO("The file must end with </feed> so we know it hasn't been cut off or whatever");
            REQUIRE(content.ends_with("</feed>\n"));
        }

        {
            INFO("There are only 9 blog posts, so we expect 9 entries");
            REQUIRE(
                tests::count(
                    content,
                    "<entry>"
                ) == 9
            );
        }
    }
    SECTION("The blog should be split into 5 pages") {
        std::filesystem::path blogRoot = testGen.buildPath() / "blog";
        auto pageRoot = blogRoot / "page";

        std::vector<std::filesystem::path> pageDirContents;
        for (auto& entry : std::filesystem::directory_iterator(pageRoot)) {
            pageDirContents.push_back(pageRoot / entry.path());
        }

        REQUIRE(pageDirContents.size() == 5);

        std::vector<std::vector<std::string>> expectedPageUrls = {
            {
                "2026-06-08-post9.html",
                "2026-06-07-post8.html",
            },
            {
                "2026-06-06-post7.html",
                "2026-06-05-post6.html",
            },
            {
                "2026-06-04-post5.html",
                "2026-06-03-post4.html",
            },
            {
                "2026-06-02-post3.html",
                "2026-06-01-post2.html",
            },
            {
                "2026-05-31-post1.html",
            },
        };

        for (size_t i = 0; i < 5; ++i) {
            auto indexFile = pageRoot / std::to_string(i + 1) / "index.html";
            REQUIRE(
                std::filesystem::exists(
                    indexFile
                )
            );

            std::ifstream f(indexFile);
            {
                INFO("Attempting to open " << indexFile.string());
                REQUIRE(bool(f));
            }

            std::string content = tests::readFile(f);
            INFO("Full content: " << content);

            // We need to filter out the sidebar to avoid those from interfering with the content tests. The sidebar
            // will otherwise act as both a pass (for positive tests) and a fail (for negative tests) for all tests
            // involving URLs
            auto mainLoc = content.find("<main>");
            REQUIRE(mainLoc != std::string::npos);
            std::string mainContentPart = content.substr(
                mainLoc
            );
            INFO("Main content part: " << mainContentPart);

            {
                INFO("There should only be 1 or 2 <article> elements per page.");
                REQUIRE(
                    tests::count(
                        content,
                        "<article>"
                    ) == expectedPageUrls.at(i).size()
                );
            }

            for (auto& url : expectedPageUrls.at(i)) {
                INFO("This page should contain "
                    << url
                    << "(Note: using main content part, not full content)"
                );
                REQUIRE(mainContentPart.contains(url));
            }
            for (size_t j = 0; j < 5; ++j) {
                if (j == i) {
                    continue;
                }

                for (auto& url : expectedPageUrls.at(j)) {
                    INFO("Page " << i
                        << " should NOT contain "
                        << url
                        << ", which should be on page "
                        << j
                        << "(Note: using main content part, not full content)"
                    );
                    REQUIRE_FALSE(mainContentPart.contains(url));
                }
            }
        }
    }
}
