#pragma once

#include "inja/environment.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/generate/FileManager.hpp"
#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"

namespace violet {

class InjaManager {
    GenerateOpts& opts;
    Config& cfg;
    FileManager& fileManager;

    inja::Environment env;
public:
    InjaManager(
        GenerateOpts& opts,
        Config& cfg,
        FileManager& fileManager
    );

    std::string renderPage(
        const std::string& fileContent,
        const std::filesystem::path& fileSource,
        const Frontmatter& fm
    );
};

}
