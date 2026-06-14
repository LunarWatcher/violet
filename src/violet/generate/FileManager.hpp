#pragma once

#include "stc/Environment.hpp"
#include "stc/minilog.hpp"
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

    bool ready = false;
public:
    FileManager(
        GenerateOpts& opts,
        Config& cfg
    ) : opts(opts),
        cfg(cfg)
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

        if (this->cfg.theme && (*cfg.theme)[0] == '_') {
            this->themeDir = std::filesystem::path(
                stc::executablePath()
            ).parent_path()
                / "../share/violet/themes"
                / *cfg.theme;
        } else if (this->cfg.theme) {
            this->themeDir = root / "_themes" / *cfg.theme;
        }
        ready = true;
    }

    void recursivelyIterateFiles(
        const std::filesystem::path& root,
        const std::function<void(const std::filesystem::directory_entry& entry)>& processor
    );

    void assertReady() {
        if (!ready) {
            [[unlikely]]
            throw std::runtime_error("Programmer error: filemanager functions invoked before imbueRoot");
        }
    }

    const std::filesystem::path& getRootFolder() {
        return root;
    }
};

}
