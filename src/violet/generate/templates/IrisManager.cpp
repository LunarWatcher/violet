#include "IrisManager.hpp"
#include "iris/environment.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/generate/ProcessedFileType.hpp"
#include "violet/paginator/Paginator.hpp"

#include <filesystem>
#include <iris/iris.hpp>
#include <stacktrace>

namespace violet {

IrisManager::IrisManager(
    Config& cfg,
    FileManager& fileManager,
    MetadataCache& metaCache
) : cfg(cfg),
    fileManager(fileManager),
    metaCache(metaCache),
    env(std::make_shared<iris::Environment>()),
    dateTimeFuncs(*this),
    fileFuncs(*this),
    langFuncs(*this),
    escapingFuncs(*this)
{
    reloadEnv();
    reloadMacros();
}

void IrisManager::reloadEnv() {
    env->set_include_callback([this](
        const std::filesystem::path& path,
        const std::string& templateName
    ) -> iris::Template {
        auto partialPath = this->fileManager.resolvePartial(templateName);
        if (partialPath) {
            return env->parse_file(
                *partialPath
            );
        }
        if (!templateName.starts_with("partials")) {
            // TODO: this does not support alternative path locations, like allowing the template directory to become
            // the defacto root. This requires an extra path existence check against the template root before lookup,
            // and is therefore trivial to implement, but I'm not sure what should take priority
            return env->parse_file(
                path / templateName
            );
        } else {
            throw iris::CallbackError("Failed to resolve partial");
        }
    });
}

void IrisManager::reloadMacros() {
    const auto locations = cfg.macroLocations();

    if (locations.empty()) {
        minilog::info("No macros to load");
        return;
    }
    minilog::info("Found {} macro locations.", locations.size());

    for (auto& loc : locations) {
        for (const auto& f : std::filesystem::recursive_directory_iterator(loc)) {
            if (!f.is_regular_file()) {
                continue;
            }

            const auto& entryPath = f.path();

            if (entryPath.extension() != ".iris") {
                continue;
            }

            minilog::debug(
                "Now loading macros from {}",
                entryPath.string()
            );

            auto tmpl = env->parse_file(
                entryPath
            );

            if (tmpl.macroStorage.empty()) {
                minilog::warn(
                    "Macro file {} does not contain any macros!",
                    entryPath.string()
                );
            } else {
                for (const auto& [name, _] : tmpl.macroStorage) {
                    minilog::debug("Loading macro {}", name);
                }
                env->accept_macros(tmpl);
            }
        }
    }
}

RenderContext IrisManager::buildCommonContext(const Frontmatter& pageFm) {
    return RenderContext {
        .fm = pageFm,
        .site = cfg.asIrisObject(),
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
        std::vector<Frontmatter> pages;

        // TODO: This isn't great :/ But it work shaped
        for (auto& page : **pagIt) {
            pages.push_back(*page);
        }

        context.listing = ListingRenderContext {
            .page = (int64_t) pagIt->getPage() + 1,
            .total_pages = (int64_t) pagIt->getTotalPages(),
            .pages = std::move(pages),
            .base_path = std::filesystem::path(pagIt->paginator.basePath).parent_path(),
        };
    }

    if (fm.hasLayout()) {
        auto templateLocation = this->fileManager.resolveTemplate(
            fm.type,
            fm.getLayout()
        );
        if (!templateLocation.has_value()) {
            if (fm.internalFileType == ProcessedFileType::Xml) {
                return env->render(
                    fileContent,
                    context,
                    (void*) &context,
                    fm.internalPath
                );
            } else {
                throw iris::CallbackError(
                    std::format(
                        "Failed to find template for type {}/layout {}",
                        fm.type,
                        fm.getLayout()
                    )
                );
            }
        }
        iris::Template content = env->parse(fileContent, fm.internalPath);
        env->include_template("content", content);
        return env->render_file(
            *templateLocation,
            context,
            (void*) &context
        );
    } else {
        return env->render(
            fileContent,
            context,
            (void*) &context,
            fm.internalPath
        );
    }
}

void IrisManager::resetCaches() {
    fileFuncs.clear();
}

}
