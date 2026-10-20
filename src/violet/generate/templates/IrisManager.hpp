#pragma once

#include "iris/environment.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/generate/FileManager.hpp"
#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/generate/cache/MetadataCache.hpp"
#include "violet/generate/templates/DateTimeFunctions.hpp"
#include "violet/generate/templates/FileFunctions.hpp"
#include "violet/generate/templates/EscapingFunctions.hpp"
#include "violet/generate/templates/LangFunctions.hpp"
#include "violet/paginator/Paginator.hpp"

namespace violet {

class IrisManager {
public:
    GenerateOpts& opts;
    Config& cfg;
    FileManager& fileManager;
    MetadataCache& metaCache;

    iris::Environment env;

public:
    DateTimeFunctions dateTimeFuncs;
    FileFunctions fileFuncs;
    LangFunctions langFuncs;
    EscapingFunctions escapingFuncs;

    IrisManager(
        GenerateOpts& opts,
        Config& cfg,
        FileManager& fileManager,
        MetadataCache& metaCache
    );

    IrisManager(IrisManager&) = delete;
    IrisManager(IrisManager&&) = delete;

    nlohmann::json buildCommonContext(const Frontmatter& pageFm);

    std::string renderPage(
        const std::string& fileContent,
        const std::filesystem::path& fileSource,
        const Frontmatter& fm,
        Paginator::iterator* pag
    );
};

}
