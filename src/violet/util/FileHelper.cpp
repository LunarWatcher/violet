#include "FileHelper.hpp"

namespace violet {

extern bool util::shouldExclude(const std::filesystem::path& path, const Config& cfg)
{
    auto normPath = std::filesystem::proximate(path, cfg.root());
    const auto& excludes = cfg.getExcludes();
    return normPath.filename().string().starts_with(".git")
           || std::find(excludes.cbegin(), excludes.cend(), normPath.string()) != excludes.cend();
}

}
