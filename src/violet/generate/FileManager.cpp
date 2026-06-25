#include "FileManager.hpp"
#include "violet/conf/ThemeConfig.hpp"

#include <filesystem>
#include <fstream>
#include <stc/FileUtil.hpp>
#include <minilog/minilog.hpp>

namespace violet {

void FileManager::copyRaw(
    const std::filesystem::path& root,
    const std::filesystem::path& relPath,
    const std::filesystem::path& destName
) {
    assertReady();
    auto fullSource = root / relPath;
    auto fullDest = opts.outputFolder / destName;

    std::filesystem::create_directories(
        fullDest.parent_path()
    );

    std::filesystem::copy(
        fullSource,
        fullDest,
        std::filesystem::copy_options::update_existing
    );
}

std::optional<std::filesystem::path> FileManager::resolveTemplate(
    const std::string& type,
    const std::string& layout
) {
    assertReady();

    std::vector<std::filesystem::path> paths;
    paths.reserve(4);

    auto file = layout + ".inja";
    // std::cout << file << std::endl;

    paths.push_back(root / "_templates" / type);
    if (cfg.theme) {
        paths.push_back(themeDir / "_templates" / type);
    }

    paths.push_back(root / "_templates" / "_default");
    if (cfg.theme) {
        paths.push_back(themeDir / "_templates" / "_default");
    }

    return stc::FileUtil::findFile(
        paths,
        file
    );
}
void FileManager::copyThemeFiles() {
    assertReady();
    if (!cfg.theme) {
        minilog::info("No theme set");
        return;
    }
    std::ifstream confFile(themeDir / "violet.theme.json");
    
    if (!confFile) {
        minilog::warn(
            "Failed to open violet.theme.json for theme. Tried path: {}",
            (themeDir / "violet.theme.json").string()
        );
        return;
    }

    minilog::info("Copying theme exports...");
    // TODO: export ThemeConfig to the class
    ThemeConfig themeCfg = nlohmann::json::parse(confFile);
    for (const auto& m : themeCfg.mount) {
        auto folder = themeDir / m;
        if (!std::filesystem::is_directory(folder)) {
            throw std::runtime_error("Misconfigured theme: tried to mount non-folder");
        }
        for (auto& file : std::filesystem::recursive_directory_iterator(folder)) {
            if (std::filesystem::is_directory(file)) {
                continue;
            }
            auto rel = std::filesystem::relative(
                file.path(),
                themeDir
            );
            minilog::debug("COPY {}", std::filesystem::weakly_canonical(themeDir / rel).string());
            copyRaw(
                themeDir,
                rel,
                rel
            );
        }
    }
}

std::optional<std::filesystem::path> FileManager::resolvePartial(
    const std::filesystem::path& path
) {
    if (!path.string().starts_with("partials/")) {
        return std::nullopt;
    }

    auto fn = path.filename().string();
    if (!fn.ends_with(".inja")) {
        fn += ".inja";
    }

    if (auto p = this->root / "_partials" / fn;
        std::filesystem::is_regular_file(p)
    ) {
        return p;
    }

    if (cfg.theme) {
        if (auto p = this->themeDir / "_partials" / fn;
            std::filesystem::is_regular_file(p)
        ) {
            return p;
        }
    }
    minilog::error(
        "Failed to resolve partial {}",
        path.string()
    );

    return std::nullopt;
}

void FileManager::recursivelyIterateFiles(
    const std::filesystem::path& root,
    const std::function<void(const std::filesystem::directory_entry& entry)>& processor
) {
    auto it = std::filesystem::recursive_directory_iterator(root);
    auto end = std::filesystem::recursive_directory_iterator();

    std::filesystem::path realOutputPath = opts.outputFolder;
    if (!realOutputPath.is_absolute()) {
        realOutputPath = this->root / opts.outputFolder;
    }
    realOutputPath = std::filesystem::weakly_canonical(realOutputPath);


    // Subfolder of output path (avoids scopes set to {output_dir}/**)
    // The implementation is shit because there is no trivial way to check if a path is a subpath of another
    // path without resorting to stuff that's unreliable. This solution is only valid for canonical paths
    // though, but that's fine, everything is canonical or made canonical here
    if (std::filesystem::canonical(root).string().starts_with(realOutputPath.string())) {
        return;
    }

    do {
        auto& f = *it;
        // Required to deal with "." as the scope in treePages and listPages
        auto p = std::filesystem::weakly_canonical(
            f.path()
        );
        auto relPath = std::filesystem::relative(
            p,
            root
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
        } // For now, skip anything that isn't a normal file. This screws symlinks, but I see that as acceptable.

        minilog::debug("Forwarding {} to file tree iterator", f.path().string());
        processor(f);
    } while (++it != end);
}

}
