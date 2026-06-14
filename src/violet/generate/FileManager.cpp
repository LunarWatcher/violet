#include "FileManager.hpp"
#include "violet/conf/ThemeConfig.hpp"

#include <filesystem>
#include <fstream>
#include <stc/FileUtil.hpp>
#include <stc/minilog.hpp>

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

std::filesystem::path FileManager::resolveTemplate(
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

    auto filePath = stc::FileUtil::findFile(
        paths,
        file
    );

    if (!filePath) {
        throw std::runtime_error("Failed to resolve " + file);
    }

    return *filePath;
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

}
