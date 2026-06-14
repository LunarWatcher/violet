#pragma once

#include "fixtures/InjaArgsGenerator.hpp"
#include "fixtures/TestSiteGenerator.hpp"
#include <filesystem>

namespace filelist {

inline void testFileIterationExcludesOutputDir(
    tests::TestSiteGenerator& gen
) {
    gen.assertGenerated();

    SECTION("File iteration methods must exclude the output directory") {
        auto& outputDir = gen.buildPath();
        auto& sourceDir = gen.sourcePath();

        auto fn = "special_test_file__testFileIterationExcludesOutputDir.md";

        {
            std::ofstream f(outputDir / fn);

            if (!f) {
                FAIL("Failed to open " << (outputDir / fn).string());
            }

            f << "content" << std::endl;
        }


        SECTION("When scope is equal to the source path") {
            auto refFile = sourceDir / "README.md";
            {
                std::ofstream f(refFile);
                if(!f) {
                    FAIL("Failed to open " << refFile.string());
                }
                f << "content" << std::endl;
            }
            tests::InjaArgs args(nlohmann::json::array({
                violet::Frontmatter {
                    .title = "Placeholder",
                    .internalPath = refFile.string()
                },
                "."
            }));
            auto injaArgs = args.asArgs();

            SECTION("listPages should exclude build dir") {
                auto pages = gen->getInjaManager().fileFuncs.listPages(injaArgs);

                for (const auto& page : pages) {
                    auto fm = page.get<violet::Frontmatter>();

                    REQUIRE_FALSE(
                        fm.internalPath.contains(fn)
                    );
                    REQUIRE_FALSE(
                        std::filesystem::canonical(
                            sourceDir / fm.internalPath
                        ).string().contains(outputDir.string())
                    );
                }
            }
            SECTION("treePages should exclude build dir") {
                auto pages = gen->getInjaManager().fileFuncs.treePages(injaArgs);

                auto subfolders = pages.at("subfolders");
                REQUIRE_FALSE(
                    subfolders.contains(outputDir.filename().string())
                );
            }
        }

        SECTION("When the scope is the build directory, the list should be empty") {
            auto refFile = sourceDir / "README.md";
            {
                std::ofstream f(refFile);
                if(!f) {
                    FAIL("Failed to open " << refFile.string());
                }
                f << "content" << std::endl;
            }
            tests::InjaArgs args(nlohmann::json::array({
                violet::Frontmatter {
                    .title = "Placeholder",
                    .internalPath = refFile.string()
                },
                outputDir.filename().string()
            }));

            INFO("Scope is " << outputDir.filename().string());
            auto injaArgs = args.asArgs();
            SECTION("listPages") {
                auto pages = gen->getInjaManager().fileFuncs.listPages(injaArgs);
                REQUIRE(pages.empty());
            }
            SECTION("treePages") {
                auto pages = gen->getInjaManager().fileFuncs.treePages(injaArgs);
                REQUIRE(pages.empty());
            }
        }

    }
}

}
