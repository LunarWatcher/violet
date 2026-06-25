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
    );

    violet::SiteGenerator* operator*() { return gen.get(); }
    violet::SiteGenerator* operator->() { return gen.get(); }

    const std::filesystem::path& buildPath() { return workspace.buildDirectory.folder; }
    const std::filesystem::path& sourcePath() { return workspace.rootDirectory; }

    void generateWithAsserts();
    void assertGenerated();
};

}
