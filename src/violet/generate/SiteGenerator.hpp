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
        const std::filesystem::path& relPath,
        ProcessedFileType type
    );

    void handleTemplatesAndSave(
        std::string&& fileContent,
        const std::filesystem::path& relPath,
        const Frontmatter& frontmatter
    );

    std::filesystem::path resolvePagination(
        const std::filesystem::path& contentRoot,
        size_t page
    );

    void renderAndWrite(
        const std::filesystem::path& file,
        const std::filesystem::path& relPath,
        const std::string& fileContent,
        const Frontmatter& frontmatter,
        Paginator::iterator* pag
    );
public:
    ~SiteGenerator() = default;

    /**
     * Generates the site
     *
     * \returns whether or not the generation was successfully completed.
     */
    bool generate();

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
