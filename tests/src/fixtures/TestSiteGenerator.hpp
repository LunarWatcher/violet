#pragma once

#include "catch2/catch_test_macros.hpp"
#include "fixtures/LoadWorkspace.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/generate/SiteGenerator.hpp"
namespace tests {

struct TestSiteGenerator {
    tests::LoadWorkspace workspace;
    violet::GenerateOpts opts;
    std::shared_ptr<violet::SiteGenerator> gen;

    TestSiteGenerator(
        SiteVariant site,
        const std::string& buildDirectoryName = "pages"
    ) :
        workspace(site, buildDirectoryName),
        opts({
            .watch = false,
            .outputFolder = workspace.rootDirectory / buildDirectoryName,
            .overridePrefixForLocalUse = true,
            .root = workspace.rootDirectory,
        })
    {
        auto result = violet::SiteGenerator::loadWorkspace(
            opts
        );
        REQUIRE(result.has_value());

        this->gen = *result;
    }

    violet::SiteGenerator* operator*() { return gen.get(); }
    violet::SiteGenerator* operator->() { return gen.get(); }

    const std::filesystem::path& buildPath() { return workspace.buildDirectory.folder; }
};

}
