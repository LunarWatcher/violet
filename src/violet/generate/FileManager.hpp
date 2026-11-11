#pragma once

#include "violet/conf/Config.hpp"
#include <filesystem>

namespace violet {

class FileManager {
private:
    Config& cfg;
    std::filesystem::path themeDir;
public:
    FileManager(Config& cfg);

    std::optional<std::filesystem::path> resolveTemplate(
        const std::string& type,
        const std::string& layout
    );

    std::optional<std::filesystem::path> resolvePartial(
        const std::filesystem::path& path
    );

    void copyRaw(
        const std::filesystem::path& root,
        const std::filesystem::path& relPath,
        const std::filesystem::path& destName
    );

    void copyThemeFiles();

    void recursivelyIterateFiles(
        const std::filesystem::path& root,
        const std::function<bool(const std::filesystem::directory_entry& entry)>& processor
    );

    const std::filesystem::path& root() const {
        return cfg.root();
    }
};

}
