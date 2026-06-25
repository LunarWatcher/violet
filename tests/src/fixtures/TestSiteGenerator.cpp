#include "TestSiteGenerator.hpp"

namespace tests {

TestSiteGenerator::TestSiteGenerator(
    SiteVariant site,
    const std::string& buildDirectoryName
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

void TestSiteGenerator::generateWithAsserts() {
    REQUIRE_FALSE(std::filesystem::exists(buildPath()));
    REQUIRE_NOTHROW(gen->generate());
    REQUIRE(std::filesystem::exists(buildPath()));
}

void TestSiteGenerator::assertGenerated() {
    INFO("gen->generate must be called before this section is declared");
    REQUIRE(std::filesystem::exists(buildPath()));
}

}
