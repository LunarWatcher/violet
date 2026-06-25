#include "InjaManager.hpp"
#include "inja/environment.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/data/VioletGlobal.hpp"
#include "violet/generate/ProcessedFileType.hpp"
#include "violet/paginator/Paginator.hpp"

#include <filesystem>
#include <inja/inja.hpp>

namespace violet {

InjaManager::InjaManager(
    GenerateOpts& opts,
    Config& cfg,
    FileManager& fileManager,
    MetadataCache& metaCache
) :
    opts(opts),
    cfg(cfg),
    fileManager(fileManager),
    metaCache(metaCache),
    fileFuncs(*this),
    langFuncs(*this),
    htmlFuncs(*this),
    dateTimeFuncs(*this)
{
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
}

std::string InjaManager::renderPage(
    const std::string& fileContent,
    const std::filesystem::path&, // TODO: what was the idea with this arg?
    const Frontmatter& fm,
    Paginator::iterator* pagIt
) {
    nlohmann::json context = {
        {"page", fm},
        {"site", this->cfg.raw},
        {"violet", violet::violet_global}
    };

    if (pagIt != nullptr) {
        std::vector<nlohmann::json> pages;

        // TODO: This isn't great :/ But it work shaped
        for (auto& page : **pagIt) {
            pages.push_back(*page);
        }

        context["listing"] = nlohmann::json::object({
            { "page", pagIt->getPage() + 1 },
            { "total_pages", pagIt->getTotalPages() },
            { "pages", std::move(pages) },
            { "base_path", std::filesystem::path(pagIt->paginator.pageFm.internalUrl).parent_path().string() },
        });
    }

    if (!fm.isAsset) {
        auto templateLocation = this->fileManager.resolveTemplate(
            fm.type,
            fm.getLayout()
        );
        if (!templateLocation.has_value()) {
            if (fm.internalFileType == ProcessedFileType::Xml) {
                return env.render(
                    fileContent,
                    context
                );
            } else {
                throw std::runtime_error(
                    std::format(
                        "Failed to find template for type {}/layout {}",
                        fm.type,
                        fm.getLayout()
                    )
                );
            }
        }
        inja::Template content = env.parse(fileContent);
        env.include_template("content", content);
        return env.render_file(
            *templateLocation,
            context
        );
    } else {
        return env.render(
            fileContent,
            context
        );
    }
}

}
