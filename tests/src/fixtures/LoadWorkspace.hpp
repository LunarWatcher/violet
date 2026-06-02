#pragma once

#include <filesystem>
#include <stc/Environment.hpp>
#include <stc/test/TestDirectory.hpp>

namespace tests {

enum class SiteVariant {
    /**
     * Standard test site with some fairly common defaults enabled. Most tests should use this site.  
     */
    BaseTestSite,

    /**
     * Standard test site modelled after a blog  
     */
    BlogTestSite,
};

struct LoadWorkspace {
    std::filesystem::path rootDirectory;
    stc::testutil::TestDirectory buildDirectory;

    [[nodiscard("Discarding this struct will immediately reset the build directory")]]
    LoadWorkspace(
        SiteVariant site,
        const std::string& buildDirectoryName = "pages"
    ) :
        rootDirectory(deriveTargetPath(site)),
        buildDirectory(rootDirectory / buildDirectoryName)
    {}

    std::filesystem::path deriveTargetPath(SiteVariant site) {
        return std::filesystem::path{TEST_ROOT_PATH} / "sites" / resolveSite(site);
    }

    std::string resolveSite(SiteVariant site) {
        switch(site) {
        case SiteVariant::BaseTestSite:
            return "test-site";
        case SiteVariant::BlogTestSite:
            return "test-blog";
        }

        throw std::runtime_error("Undefined");
    }
};

}
