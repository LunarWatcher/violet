#pragma once

#include "fixtures/IrisArgsGenerator.hpp"
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
            tests::IrisArgs args(iris::Array {
                violet::Frontmatter {
                    .title = "Placeholder",
                    .internalPath = refFile.string()
                },
                "."
            });
            auto irisArgs = args.asArgs();

            SECTION("listPages should exclude build dir") {
                auto rawPages = gen->getIrisManager()
                    .fileFuncs
                    .listPages(irisArgs);
                INFO(rawPages.serialize());
                REQUIRE(rawPages.is_array());
                auto pages = rawPages.get<iris::Array>();

                for (const auto& page : pages) {
                    INFO(page.serialize());
                    std::cerr << page.serialize() << std::endl;
                    auto fm = violet::Frontmatter::fromDataObject(
                        page.get<iris::Object>()
                    );

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
                auto pages = gen->getIrisManager().fileFuncs.treePages(irisArgs);

                auto subfolders = pages.at("subfolders");
                REQUIRE(subfolders.is_array());
                auto arr = subfolders.get<iris::Array>();
                INFO(subfolders.serialize());
                INFO("We do not want to find folder_name = " << outputDir.filename());
                REQUIRE(
                    std::find_if(
                        arr.begin(), arr.end(),
                        [&](const iris::DataObject& it) -> bool {
                            REQUIRE(it.contains("folder_name"));
                            return it.at("folder_name").get<iris::String>() == outputDir.filename().string();
                        }
                    ) == arr.end()
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
            tests::IrisArgs args(iris::Array {
                violet::Frontmatter {
                    .title = "Placeholder",
                    .internalPath = refFile.string()
                },
                outputDir.filename().string()
            });

            INFO("Scope is " << outputDir.filename().string());
            auto irisArgs = args.asArgs();
            SECTION("listPages") {
                auto pages = gen->getIrisManager().fileFuncs.listPages(irisArgs);
                REQUIRE(pages.empty());
            }
            SECTION("treePages") {
                auto pages = gen->getIrisManager().fileFuncs.treePages(irisArgs);
                REQUIRE(pages.empty());
            }
        }

    }
}

}
