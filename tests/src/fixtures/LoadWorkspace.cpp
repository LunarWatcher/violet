#include "LoadWorkspace.hpp"

namespace tests {

std::string LoadWorkspace::resolveSite(SiteVariant site) {
    switch(site) {
    case SiteVariant::BaseTestSite:
        return "test-site";
    case SiteVariant::BlogTestSite:
        return "test-blog";
    }

    throw std::runtime_error("Undefined");
}

}
