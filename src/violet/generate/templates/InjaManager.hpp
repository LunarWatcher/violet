#pragma once

#include "inja/environment.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/generate/FileManager.hpp"
#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/generate/cache/MetadataCache.hpp"

namespace violet {

class InjaManager {
    GenerateOpts& opts;
    Config& cfg;
    FileManager& fileManager;
    MetadataCache& metaCache;

    inja::Environment env;
public:
    InjaManager(
        GenerateOpts& opts,
        Config& cfg,
        FileManager& fileManager,
        MetadataCache& metaCache
    );

    std::string renderPage(
        const std::string& fileContent,
        const std::filesystem::path& fileSource,
        const Frontmatter& fm
    );
};

}
