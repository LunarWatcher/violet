#include "SiteGenerator.hpp"

#include "ProcessedFileType.hpp"

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
    injaManager(
        this->opts,
        this->cfg,
        this->fileManager
    )
{}

nlohmann::json SiteGenerator::parseFrontmatter(std::ifstream& in) {
    // we need to re-do the check already done on HTML, since we allow markdown to not contain frontmatter
    std::string lineBuff;
    std::getline(in, lineBuff);

    if (lineBuff != "---") {
        // Markdown; frontmatter is empty. Reset input stream
        in.seekg(0);
        in.clear();
        return nlohmann::json::object();
    }
    std::stringstream json;
    while (std::getline(in, lineBuff)) {
        if (lineBuff == "---") {
            if (json.tellp() == 0) {
                // No JSON
                // This is not particularly robust, but good enough for now.
                // Parts of this will be substituted by an error once the parsing is error-handled.
                return nlohmann::json::object();
            }
            // end of frontmatter reached
            nlohmann::json out;
            std::cout << json.str() << std::endl;
            json >> out;
            return out;
        }
        json << lineBuff << std::endl;
    }
    // EOF with no frontmatter terminator. Return null
    // TODO: ... and log an error for the user.
    // TODO: do we also want to output to file, or just log at that point? I'm in favour of CLI errors, but I'm not sure
    // if that scales well if there's hundreds of errors. I am somewhat in favour of a report file of sorts though, that
    // compiles an aggregate set of the errors and where they're at.
    return nullptr;
}

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
    std::ifstream in(rootDir / relPath);
    nlohmann::json frontmatter = parseFrontmatter(in);

    if (frontmatter == nullptr) {
        return false;
    }
    Frontmatter parsedFrontmatter = frontmatter;
    parsedFrontmatter.withFilePath(relPath);

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
            content
        );

        std::filesystem::path newPath;
        if (relPath.filename() != "README.md") {
            newPath = relPath.replace_extension(".html");
        } else {
            newPath = relPath.replace_filename("index.html");
        }

        handleTemplatesAndSave(
            std::move(fileContent),
            newPath,
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

    auto it = std::filesystem::recursive_directory_iterator(rootDir);
    auto end = std::filesystem::recursive_directory_iterator();

    // Not a huge fan of this
    std::filesystem::path realOutputPath = opts.outputFolder;
    if (!realOutputPath.is_absolute()) {
        realOutputPath = rootDir / opts.outputFolder;
    }
    if (opts.overridePrefixForLocalUse) {
        std::cout << "Overriding prefix: using " << realOutputPath.string() << std::endl;
        cfg.raw["site_prefix"] = realOutputPath.string();
    }


    fileManager.copyThemeFiles();

    bool success = true;
    do {

        auto& f = *it;
        auto& p = f.path();
        auto relPath = std::filesystem::relative(
            p,
            rootDir
        );
        if (f.is_directory()) {
            if (
                // Ignore git
                p.filename().string().starts_with(".git")
                // Ignore user excludes
                || std::find(
                    cfg.exclude.cbegin(),
                    cfg.exclude.cend(),
                    relPath.string()
                ) != cfg.exclude.cend()
                // Inline output directory
                || p == realOutputPath
            ) {
                it.disable_recursion_pending();
                continue;
            }

            // The special _ directories at the root are not processed by the recursion logic, so it must be disabled
            // here.
            // We also blanket disable all <root>/_[dir] folders since these are reserved folders.
            if (relPath.string().starts_with("_")) {
                it.disable_recursion_pending();
                continue;
            }
        } else if (f.is_regular_file()) {
            if (
                // Ignore git
                p.filename().string().starts_with(".git")
                // Ignore violet config files
                || relPath.string() == "violet.json"
                || relPath.string() == "violet.theme.json"
                // Ignore user excludes
                || std::find(
                    cfg.exclude.cbegin(),
                    cfg.exclude.cend(),
                    relPath.string()
                ) != cfg.exclude.cend()
            ) {
                continue;
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
                    continue;
                }

                success &= processFile(rootDir, relPath, ProcessedFileType::Markdown);
                continue;
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
                    continue;
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
                continue;
            } else if (ext == ".js" || ext == ".mjs" || ext == ".css") {
                // TODO: handle specially
            }
        cont:
            fileManager.copyRaw(rootDir, relPath, relPath);

        } // For now, skip anything that isn't a normal file. This screws symlinks, but I see that as acceptable.

        // std::cout << "Path: " << relPath.string() << std::endl;
    } while (++it != end);

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
