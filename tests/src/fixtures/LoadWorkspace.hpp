#pragma once

#include <filesystem>
#include <stc/Environment.hpp>
#include <utility>


namespace tests {

enum SiteVariant {
    BaseTestSite
};

struct LoadWorkspace {
    std::filesystem::path oldPwd;
    LoadWorkspace(SiteVariant site, bool dry = false) {
        oldPwd = std::filesystem::current_path();
        if (!dry) {
            std::filesystem::current_path(
                deriveTargetPath(site)
            );
        }
    }
    ~LoadWorkspace() {
        std::filesystem::current_path(oldPwd);
    }

    std::filesystem::path deriveTargetPath(SiteVariant site) {
        return std::filesystem::path{TEST_ROOT_PATH} / "sites" / resolveSite(site);
    }
private:
    std::string resolveSite(SiteVariant site) {
        switch(site) {
        case SiteVariant::BaseTestSite:
            return "test-site";
        }

        std::unreachable();
    }
};

}
