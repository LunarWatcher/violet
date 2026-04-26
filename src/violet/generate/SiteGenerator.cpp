#include "SiteGenerator.hpp"

#include "ProcessedFileType.hpp"
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
void SiteGenerator::handleTemplatesAndSave(
    std::string&& fileContent,
    const std::filesystem::path& relPath,
    const Frontmatter& frontmatter
) {
    auto target = this->opts.outputFolder / relPath;
    std::filesystem::create_directories(
        target.parent_path()
    );
    std::ofstream f(
        target
    );
    if (!f) {
        std::cerr << "Failed to open " << target << std::endl;
        return;
    }
    
    auto renderedContent = injaManager.renderPage(
        fileContent,
        relPath,
        frontmatter
    );
    
    std::cout << "Committing generated page " << target << std::endl;
    f.write(
        renderedContent.data(),
        renderedContent.size()
    );
}

bool SiteGenerator::processFile(
    const std::filesystem::path& rootDir,
    std::filesystem::path relPath,
    ProcessedFileType type
) {
    std::cout << "Now reading " << relPath << std::endl;
    auto data = this->metadataCache.openFile(
        relPath
    );

    auto& parsedFrontmatter = data.metadata.frontmatter;
    auto& in = data.file;

    std::stringstream content;
    content << in.rdbuf();

    switch (type) {
    case ProcessedFileType::Html: {
        std::string fileContent = content.str();

        handleTemplatesAndSave(
            std::move(fileContent),
            relPath,
            parsedFrontmatter
        );
    } break;
    case ProcessedFileType::Markdown: {
        std::string fileContent = Markdown::parse(
            content,
            std::bind(
                projectBasedTranslator,
                this->cfg.site_prefix.value_or(""),
                rootDir,
                relPath,
                std::placeholders::_1
            )
        );

        handleTemplatesAndSave(
            std::move(fileContent),
            parsedFrontmatter.internalUrl,
            parsedFrontmatter
        );
    } break;
    }

    return true;
}

bool SiteGenerator::generate(
    const std::filesystem::path& rootDir
) {
    this->fileManager.imbueRoot(rootDir);
    this->cfg.imbueRoot(rootDir);

    std::filesystem::path realOutputPath = opts.outputFolder;
    if (!realOutputPath.is_absolute()) {
        realOutputPath = rootDir / opts.outputFolder;
    }
    if (opts.overridePrefixForLocalUse) {
        std::cout << "Overriding prefix: using " << realOutputPath.string() << std::endl;
        cfg.raw["site_prefix"] = realOutputPath.string();
        cfg.site_prefix = realOutputPath.string();
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
                // HTML files are conditionally processed. No frontmatter means they're copied as any other static file
                // would, frontmatter means processed.
                //
                // Here, we do a manual read to avoid `std::getline`, just in case it's a minimised file. This reads 7
                // bytes rather than potentially an entire gigabyte-sized single-line file (unlikely to ever come up,
                // but might as well)
                std::ifstream in(p);
                // this means the file is not readable. copyRaw will throw in this case.
                // Other files can also trigger the same error, but we shamelessly ignore those for now.
                if (!in) {
                    // TODO: warn
                    return;
                }

                char ch;
                for (size_t i = 0; i < 3; ++i) {
                    if (!(in >> ch) || ch != '-') {
                        goto cont;
                    }
                }
                // noskipws is required, or the char will be eaten automagically by features that otherwise would be a
                // convenience
                if (!(in >> std::noskipws >> ch) || (ch != '\n' && ch != '\r')) {
                    goto cont;
                }

                in.close();
                // we have a frontmatter, or at least something that is frontmatter-shaped.
                // It may still be malformed, but this will be addressed by processFile()
                success &= processFile(rootDir, relPath, ProcessedFileType::Html);
                return;
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
    std::ifstream f("violet.json");

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
