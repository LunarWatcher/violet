#pragma once

#include "stc/Environment.hpp"
#include "minilog/minilog.hpp"
#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"
#include <filesystem>

namespace violet {

class FileManager {
private:
    Config& cfg;
    std::filesystem::path themeDir;
public:
    FileManager(
        Config& cfg
    ) : cfg(cfg) {
        // TODO: error `stc::executablePath` should probably be cached statically to better support rebuilds
        if (this->cfg.theme && (*cfg.theme)[0] == '_') {
            this->themeDir = std::filesystem::path(
                stc::executablePath()
            ).parent_path()
                / "../share/violet/themes"
                / *cfg.theme;
        } else if (this->cfg.theme) {
            this->themeDir = cfg.root() / "_themes" / *cfg.theme;
        }
    }

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
