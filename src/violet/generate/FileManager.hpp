#pragma once

#include "stc/Environment.hpp"
#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"
#include <filesystem>

namespace violet {

class FileManager {
private:
    GenerateOpts& opts;
    Config& cfg;
    std::filesystem::path root;
    std::filesystem::path themeDir;

    bool hasTheme;
    bool ready = false;
public:
    FileManager(
        GenerateOpts& opts,
        Config& cfg
    ) : opts(opts),
        cfg(cfg), 
        hasTheme(cfg.theme.has_value())
    {}

    std::filesystem::path resolveTemplate(
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

    void imbueRoot(
        const std::filesystem::path& root
    ) {
        this->root = root;

        // TODO: this is currently only valid for running in the git repo
        if (this->hasTheme && (*cfg.theme)[0] == '_') {
            this->themeDir = std::filesystem::path(
                stc::executablePath()
            ).parent_path()
                / "../../themes"
                / *cfg.theme;
        } else if (this->hasTheme) {
            this->themeDir = root / "_themes" / *cfg.theme;
        }
        ready = true;
    }

    void assertReady() {
        if (!ready) {
            [[unlikely]]
            throw std::runtime_error("Programmer error: filemanager functions invoked before imbueRoot");
        }
    }
};

}
