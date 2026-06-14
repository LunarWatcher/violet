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
    ) {
        auto it = std::filesystem::recursive_directory_iterator(root);
        auto end = std::filesystem::recursive_directory_iterator();

        std::filesystem::path realOutputPath = opts.outputFolder;
        if (!realOutputPath.is_absolute()) {
            realOutputPath = this->root / opts.outputFolder;
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
