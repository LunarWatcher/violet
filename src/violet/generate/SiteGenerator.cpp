#include "SiteGenerator.hpp"

#include "ProcessedFileType.hpp"
#include "minilog/minilog.hpp"
#include "violet/paginator/Paginator.hpp"
#include "violet/parsing/LinkTranslate.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <violet/parsing/Markdown.hpp>

namespace violet {

SiteGenerator::SiteGenerator(
    const GenerateOpts& opts,
    Config&& workspaceConf
) : opts(opts),
    cfg(std::move(workspaceConf)),
    fileManager(
        this->opts,
        this->cfg
    ),
    metadataCache(
        this->fileManager
    ),
    injaManager(
        this->opts,
        this->cfg,
        this->fileManager,
        this->metadataCache
    )
{}


std::filesystem::path SiteGenerator::resolvePagination(
    const std::filesystem::path& contentRoot,
    size_t page,
    bool createDirs
) {
    auto target = contentRoot / "page" / std::to_string(page + 1) / "index.html";
    if (createDirs) {
        std::filesystem::create_directories(
            target.parent_path()
        );
    }
    return std::move(target);
}

void SiteGenerator::renderAndWrite(
    const std::filesystem::path& file,
    const std::filesystem::path& relPath,
    const std::string& fileContent,
    const Frontmatter& frontmatter,
    Paginator::iterator* pag
) {
    std::ofstream f(
        file
    );
    if (!f) {
        minilog::error("Failed to open {}", file.string());
        return;
    }
    auto renderedContent = injaManager.renderPage(
        fileContent,
        relPath,
        frontmatter,
        pag
    );

    minilog::info("Committing generated page to {}", file.string());
    f.write(
        renderedContent.data(),
        renderedContent.size()
    );
}

void SiteGenerator::handleTemplatesAndSave(
    std::string&& fileContent,
    const std::filesystem::path& relPath,
    const Frontmatter& frontmatter
) {
    minilog::debug("Rendering templates of {}", relPath.string());
    auto target = this->opts.outputFolder / relPath;
    std::filesystem::create_directories(
        target.parent_path()
    );

    if (frontmatter.listing.has_value()) {
        minilog::debug(
            "Page is paginated page_list, expanding pages"
        );
        auto pag = Paginator {
            frontmatter,
            this->fileManager,
            metadataCache
        };
        for (auto it = pag.begin(); it != pag.end(); ++it) {
            if (it.getPage() == 0) {
                // TODO: it should be possible for <prefix>/page/1/index.html to rel="canonical" to <prefix>/index.html
                renderAndWrite(
                    target,
                    relPath,
                    fileContent,
                    frontmatter,
                    &it
                );
            }
            // Not great, but need to pass the URL through somehow. Would be better to pass it through the render
            // context so we avoid this, especially considering `file` already contains all the context once the page
            // generation directory is stripped. The metadata cache will still be an issue though, since other pages
            // need to reference the page.
            Frontmatter fmCopy = frontmatter;
            fmCopy.internalUrl = resolvePagination(
                std::filesystem::path(fmCopy.internalUrl).parent_path(),
                it.getPage(),
                false
            ).string();
            renderAndWrite(
                resolvePagination(
                    target.parent_path(),
                    it.getPage()
                ),
                relPath,
                fileContent,
                fmCopy,
                &it
            );
        }
    } else {
        renderAndWrite(
            target,
            relPath,
            fileContent,
            frontmatter,
            nullptr
        );
    }
}

bool SiteGenerator::processFile(
    const std::filesystem::path& rootDir,
    const std::filesystem::path& relPath,
    ProcessedFileType type
) {
    minilog::debug("Now reading {}", relPath.string());
    auto data = this->metadataCache.openFile(
        relPath
    );

    auto& parsedFrontmatter = data.metadata.frontmatter;
    auto& in = data.file;

    std::stringstream content;
    content << in.rdbuf();

    switch (type) {
    case ProcessedFileType::Html: {
        minilog::debug("Loaded file is HTML with frontmatter.");
        std::string fileContent = content.str();

        handleTemplatesAndSave(
            std::move(fileContent),
            relPath,
            parsedFrontmatter
        );
    } break;
    case ProcessedFileType::Markdown: {
        minilog::debug("Loaded file is markdown. Parsing...");
        auto parsedPage = Markdown::parseWithContentPostprocessing(
            content,
            std::bind(
                projectBasedTranslator,
                this->cfg.prefix,
                rootDir,
                relPath,
                std::placeholders::_1
            )
        );

        minilog::debug("Markdown parsing complete");

        // TODO: this isn't elegant, but it seems to be the easiest option for now. The alternative is injecting a
        // fourth data object that's only present for markdown pages, but that just sucks.
        // Also not sure how I want to indicate that a ToC exists, but I don't think that level of markdown mixing is
        // ever going to be a problem.
        Frontmatter fm = parsedFrontmatter;
        fm.tableOfContents = std::move(parsedPage.tableOfContents);

        handleTemplatesAndSave(
            std::move(parsedPage.parsedContents),
            fm.internalUrl,
            fm
        );
    } break;
    }

    return true;
}

std::optional<bool> SiteGenerator::processFileIfFrontmatterIsPresent(
    const std::filesystem::path& file,
    const std::filesystem::path& rootDir,
    const std::filesystem::path& relPath,
    ProcessedFileType fileType
) {
    // Here, we do a manual read to avoid `std::getline`, just in case it's a minimised file. This reads 7
    // bytes rather than potentially an entire gigabyte-sized single-line file (unlikely to ever come up,
    // but might as well)
    std::ifstream in(file);
    // this means the file is not readable. copyRaw will throw in this case.
    // Other files can also trigger the same error, but we shamelessly ignore those for now.
    if (!in) {
        minilog::warn("Failed to open {} for a file read check", file.string());
        return false;
    }

    // This checks that the file starts with the frontmatter identifier.
    char ch;
    for (size_t i = 0; i < 3; ++i) {
        if (!(in >> ch) || ch != '-') {
            return std::nullopt;
        }
    }
    // noskipws is required, or the char will be eaten automagically by features that otherwise would be a
    // convenience
    // This checks that the character immediately after the --- is a newline
    if (!(in >> std::noskipws >> ch) || (ch != '\n' && ch != '\r')) {
        return std::nullopt;
    }

    in.close();

    // At this point, we have something frontmatter-shaped.
    return processFile(
        rootDir,
        relPath,
        fileType
    );
}

bool SiteGenerator::generate() {
    const auto& rootDir = this->opts.root;

    minilog::info(
        "Using {} as the project root",
        rootDir.string()
    );
    this->fileManager.imbueRoot(rootDir);
    this->cfg.imbueRoot(rootDir);

    std::filesystem::path realOutputPath = opts.outputFolder;
    if (!realOutputPath.is_absolute()) {
        realOutputPath = rootDir / opts.outputFolder;
    }
    if (opts.overridePrefixForLocalUse) {
        minilog::info(
            "Overriding site prefix to {}",
            realOutputPath.string()
        );
        cfg.raw["prefix"] = realOutputPath.string();
        cfg.prefix = realOutputPath.string();
    }


    fileManager.copyThemeFiles();

    bool success = true;
    fileManager.recursivelyIterateFiles(
        rootDir, [&](auto& f) {
            auto& p = f.path();
            auto relPath = std::filesystem::relative(
                p,
                rootDir
            );
            if (!f.is_regular_file()) {
                return;
            }
            auto ext = p.extension();

            if (ext == ".md") {

                if (
                    p.filename() == "README.md"
                    && std::filesystem::is_regular_file(
                        p.parent_path() / "index.md"
                    )
                ) {
                    // index.md takes precedence, so skip README
                    return;
                }

                success &= processFile(rootDir, relPath, ProcessedFileType::Markdown);
                return;
            } else if (ext == ".html") {
                // we have a frontmatter, or at least something that is frontmatter-shaped.
                // It may still be malformed, but this will be addressed by processFile()
                auto result = processFileIfFrontmatterIsPresent(
                    p,
                    rootDir,
                    relPath,
                    ProcessedFileType::Html
                );
                if (result.has_value()) {
                    success &= *result;
                    return;
                }
                // If the result has no value, fall back to standard copyRaw
            } else if (ext == ".js" || ext == ".mjs" || ext == ".css") {
                // TODO: handle specially
            }
        cont:
            fileManager.copyRaw(rootDir, relPath, relPath);
        }
    );

    return success;
}

std::expected<std::shared_ptr<SiteGenerator>, std::string_view> SiteGenerator::loadWorkspace(
    const GenerateOpts& opts
) {
    nlohmann::json rawConf;
    std::ifstream f(opts.root / "violet.json");

    if (!f) {
        return std::unexpected(
            "No violet.json in current working directory."
        );
    }

    f >> rawConf;
    // TODO: error handle, though nlohmann/json is very lacking in end-user error messages.
    // Might be more doable with C++26 and reflection?
    // This will currently throw
    Config cfg = rawConf;

    // Can't use the other signature, as it'll try to invoke a private constructor from a different class
    // TODO: use friend? Not sure it matters enough to bother
    return std::shared_ptr<SiteGenerator>(new SiteGenerator(
        opts,
        std::move(cfg)
    ));
}

}
