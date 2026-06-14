#include "fixtures/LoadWorkspace.hpp"
#include "fixtures/TestSiteGenerator.hpp"
#include "subtests/FileListTests.hpp"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <thread>
#include <violet/generate/SiteGenerator.hpp>

TEST_CASE("Basic site tests") {
    tests::TestSiteGenerator testGen {
        tests::SiteVariant::BaseTestSite
    };
    REQUIRE_FALSE(
        std::filesystem::exists(testGen.buildPath())
    );

    SECTION("Verify path loading") {
        REQUIRE_FALSE(
            testGen.buildPath().string().contains(std::filesystem::current_path().string())
        );
    }

    SECTION("Generating should work") {
        REQUIRE_NOTHROW(testGen->generate());

        REQUIRE(
            std::filesystem::exists(testGen.buildPath())
        );

        filelist::testFileIterationExcludesOutputDir(testGen);

        SECTION("Secrets should be excluded") {
            INFO("The secrets folder is explicitly excluded in violet.conf");
            REQUIRE(
                !std::filesystem::exists(
                    testGen.buildPath() / "secrets"
                )
            );
        }
        SECTION("z_after_everything_else should be correctly parsed") {
            REQUIRE(
                std::filesystem::exists(
                    testGen.buildPath() / "z_after_everything_else"
                )
            );
            {
                INFO("z_after_everything_else contains a README that should be converted to index.html");
                REQUIRE(
                    std::filesystem::exists(
                        testGen.buildPath() / "z_after_everything_else" / "index.html"
                    )
                );
            }
        }

        SECTION("a_folder should be parsed correctly, and include a table of contents") {
            auto testFile = testGen.buildPath() / "a_folder" / "file.html";
            REQUIRE(std::filesystem::exists(testFile));

            std::ifstream f(testFile);
            REQUIRE(bool(f));

            std::string contents;
            std::string buff;

            while (std::getline(f, buff)) {
                contents += buff;
            }

            REQUIRE(contents.contains(R"(<div id="violet-toc">)"));
            REQUIRE(contents.contains(R"(<a href="#header-1">Header 1</a>)"));
        }

        SECTION("The default theme assets should be included") {
            REQUIRE(
                std::filesystem::exists(testGen.buildPath() / "assets" / "_default_page.js")
            );
            REQUIRE(
                std::filesystem::exists(testGen.buildPath() / "assets" / "_default_style.css")
            );
            REQUIRE(
                std::filesystem::exists(testGen.buildPath() / "assets" / "vendor" / "_default-highlightjs.js")
            );
        }

        SECTION("formatDate() should work") {
            std::ifstream f(
                testGen.buildPath() / "tests/inja/formatdate.html"
            );
            REQUIRE(bool(f));

            std::string contents, buff;
            while (std::getline(f, buff)) {
                contents += buff;
            }

            INFO(contents);
            REQUIRE(contents.contains("Year: 2026"));
            REQUIRE(contents.contains("YMD: 2026&05&20"));
            REQUIRE(contents.contains("UTC+0: 2026-05-20 20:27:59+00:00"));
            REQUIRE(contents.contains("UTC+2: 2026-05-20 22:27:59+02:00"));
        }
    }
}
