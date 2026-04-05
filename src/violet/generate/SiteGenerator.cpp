#include "SiteGenerator.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

namespace violet {
SiteGenerator::SiteGenerator(
    const GenerateOpts& opts,
    Config&& workspaceConf
) : opts(opts),
    cfg(std::move(workspaceConf)),
    fileManager(this->opts)
{}

bool SiteGenerator::generate(
    const std::filesystem::path& rootDir
) {

    auto it = std::filesystem::recursive_directory_iterator(rootDir);
    auto end = std::filesystem::recursive_directory_iterator();

    // Not a huge fan of this
    std::filesystem::path realOutputPath = opts.outputFolder;
    if (!realOutputPath.is_absolute()) {
        realOutputPath = rootDir / opts.outputFolder;
    }

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

            if (ext == ".md" || ext == ".html") {
                
            } else if (ext == ".js" || ext == ".mjs" || ext == ".css") {
                // TODO: handle specially
                fileManager.copyRaw(rootDir, relPath, relPath);
            } else {
                fileManager.copyRaw(rootDir, relPath, relPath);
            }

        } // For now, skip anything that isn't a normal file. This screws symlinks, but I see that as acceptable.

        std::cout << "Path: " << relPath.string() << std::endl;
    } while (++it != end);

    return false;
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
