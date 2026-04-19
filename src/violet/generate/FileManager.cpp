#include "FileManager.hpp"
#include "stc/Environment.hpp"

#include <stc/FileUtil.hpp>

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
    if (hasTheme) {
        paths.push_back(themeDir / "_templates" / type);
    }

    paths.push_back(root / "_templates" / "_default");
    if (hasTheme) {
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

std::filesystem::path FileManager::resolvePartial(
    const std::filesystem::path& path
) {
    if (!path.string().starts_with("partials/")) {
        throw std::runtime_error("Failed to resolve " + path.string());
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

    if (hasTheme) {
        if (auto p = this->themeDir / "_partials" / fn;
            std::filesystem::is_regular_file(p)
        ) {
            return p;
        }
    }

    throw std::runtime_error("Failed to resolve " + path.string());
}

}
