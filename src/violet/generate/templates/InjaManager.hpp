#pragma once

#include "inja/environment.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/generate/FileManager.hpp"
#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/generate/cache/MetadataCache.hpp"
#include "violet/generate/templates/FileFunctions.hpp"
#include "violet/generate/templates/LangFunctions.hpp"

namespace violet {

class InjaManager {
public:
    GenerateOpts& opts;
    Config& cfg;
    FileManager& fileManager;
    MetadataCache& metaCache;

    inja::Environment env;

private:
    FileFunctions fileFuncs;
    LangFunctions langFuncs;

public:
    InjaManager(
        GenerateOpts& opts,
        Config& cfg,
        FileManager& fileManager,
        MetadataCache& metaCache
    );

    InjaManager(InjaManager&) = delete;
    InjaManager(InjaManager&&) = delete;

    std::string renderPage(
        const std::string& fileContent,
        const std::filesystem::path& fileSource,
        const Frontmatter& fm
    );
};

}
