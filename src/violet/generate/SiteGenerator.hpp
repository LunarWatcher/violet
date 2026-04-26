#pragma once

#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/generate/FileManager.hpp"
#include "violet/generate/ProcessedFileType.hpp"
#include "violet/generate/cache/MetadataCache.hpp"
#include "violet/generate/templates/InjaManager.hpp"

#include <expected>
#include <filesystem>
#include <memory>
#include <string_view>

namespace violet {

class SiteGenerator {
private:
    GenerateOpts opts;
    Config cfg;

    FileManager fileManager;
    MetadataCache metadataCache;
    InjaManager injaManager;

    SiteGenerator(
        const GenerateOpts& opts,
        Config&& workspaceConf
    );

    bool processFile(
        const std::filesystem::path& rootDir,
        std::filesystem::path relPath,
        ProcessedFileType type
    );

    void handleTemplatesAndSave(
        std::string&& fileContent,
        const std::filesystem::path& relPath,
        const Frontmatter& frontmatter
    );
public:
    ~SiteGenerator() = default;

    /**
     * Generates the site
     *
     * \param rootDir   Defines where the violet root is. Note that this is only set to the current working
     *                  directory, as options for setting it on the CLI have not been added. It's kept as a parameter
     *                  to be reserved as a future variable.
     *
     * \returns whether or not the generation was successfully completed.
     */
    bool generate(
        const std::filesystem::path& rootDir = std::filesystem::current_path()
    );

    /**
     * Returns a SiteGenerator initialised with the provided GenerateOpts, provided a `violet.json` is found in the
     * current working directory.
     * Otherwise, or in the event of any number of other init errors related to config, a string representing a type of
     * error is returned. This string should not be parsed programmatically, as it's intended for non-recoverable issues
     * that require user intervention.
     */
    static std::expected<std::shared_ptr<SiteGenerator>, std::string_view> loadWorkspace(
        const GenerateOpts& opts
    );
};

}
