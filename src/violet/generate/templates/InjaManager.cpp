#include "InjaManager.hpp"
#include "inja/environment.hpp"
#include "violet/conf/Frontmatter.hpp"

#include <filesystem>
#include <inja/inja.hpp>

namespace violet {

InjaManager::InjaManager(
    GenerateOpts& opts,
    Config& cfg,
    FileManager& fileManager,
    MetadataCache& metaCache
) : opts(opts), cfg(cfg), fileManager(fileManager), metaCache(metaCache) {
    env.set_include_callback([this](
            const std::filesystem::path& path,
            const std::string& templateName
    ) -> inja::Template {
        auto partialPath = this->fileManager.resolvePartial(templateName);
        if (partialPath) {
            return env.parse_file(
                *partialPath
            );
        }
        if (!templateName.starts_with("partials")) {
            return env.parse_file(
                path / templateName
            );
        } else {
            throw std::runtime_error("Failed to resolve partial");
        }
    });

    env.add_callback("listPages", 2, [this](inja::Arguments& args) -> nlohmann::json {
        auto& frontmatter = *args.at(0);
        auto scope = args.at(1)->get<std::string>();

        if (scope.contains("..")) {
            throw std::runtime_error(
                "listPages does not accept paths that recurse upwards"
            );
        }

        // TODO: error handling
        auto path = this->fileManager.getRootFolder() / std::filesystem::path {
            frontmatter.at("violet_internals.path")
        };
        if (std::filesystem::is_regular_file(path)) {
            path = path.parent_path();
        }
        std::cout << "Iterating " << path << std::endl;

        nlohmann::json pages;
        this->fileManager.recursivelyIterateFiles(
            path,
            [&](auto& entry) {
                
                auto entryPath = entry.path();
                if (!entry.is_regular_file()
                    || (
                        entryPath.extension() != ".md"
                        && entryPath.extension() != ".html"
                    )
                    || entryPath.filename() == "404.html"
                   ){
                    return;
                }
                auto filePath = std::filesystem::relative(
                    entryPath,
                    this->fileManager.getRootFolder()
                );
                auto& metadata = this->metaCache.loadMetadata(
                    filePath
                );
                pages.push_back(metadata.frontmatter);
            }
        );
        

        return pages;
    });
}

std::string InjaManager::renderPage(
    const std::string& fileContent,
    const std::filesystem::path& fileSource,
    const Frontmatter& fm
) {
    nlohmann::json context = {
        {"page", fm},
        {"site", this->cfg.raw},
    };

    inja::Template content = env.parse(fileContent);
    env.include_template("content", content);

    return env.render_file(
        this->fileManager.resolveTemplate(
            fm.type,
            fm.layout
        ),
        context
    );
}

}
