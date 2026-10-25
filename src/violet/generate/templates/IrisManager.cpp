#include "IrisManager.hpp"
#include "iris/environment.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/data/VioletGlobal.hpp"
#include "violet/generate/ProcessedFileType.hpp"
#include "violet/paginator/Paginator.hpp"

#include <filesystem>
#include <iris/iris.hpp>

namespace violet {

IrisManager::IrisManager(
    GenerateOpts& opts,
    Config& cfg,
    FileManager& fileManager,
    MetadataCache& metaCache
) :
    opts(opts),
    cfg(cfg),
    fileManager(fileManager),
    metaCache(metaCache),
    dateTimeFuncs(*this),
    fileFuncs(*this),
    langFuncs(*this),
    escapingFuncs(*this)
{
    env.set_include_callback([this](
        const std::filesystem::path& path,
        const std::string& templateName
    ) -> iris::Template {
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

iris::DataObject IrisManager::buildCommonContext(const Frontmatter& pageFm) {
    return iris::Object {
        {"page", pageFm},
        {"site", this->cfg.raw},
        {"violet", violet::violet_global}
    };
}

std::string IrisManager::renderPage(
    const std::string& fileContent,
    const std::filesystem::path&, // TODO: what was the idea with this arg?
    const Frontmatter& fm,
    Paginator::iterator* pagIt
) {
    auto context = buildCommonContext(fm);

    if (pagIt != nullptr) {
        std::vector<iris::DataObject> pages;

        // TODO: This isn't great :/ But it work shaped
        for (auto& page : **pagIt) {
            pages.push_back(*page);
        }

        context["listing"] = iris::Object{
            { "page", pagIt->getPage() + 1 },
            { "total_pages", pagIt->getTotalPages() },
            { "pages", std::move(pages) },
            { "base_path", std::filesystem::path(pagIt->paginator.basePath).parent_path().string() },
        };
    }

    if (fm.hasLayout()) {
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
        iris::Template content = env.parse(fileContent);
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
