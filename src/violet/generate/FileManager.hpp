#pragma once

#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"
#include <filesystem>

namespace violet {

class FileManager {
private:
    GenerateOpts& opts;
public:
    FileManager(GenerateOpts& opts) : opts(opts) {}

    void copyRaw(
        const std::filesystem::path& root,
        const std::filesystem::path& relPath,
        const std::filesystem::path& destName
    );
};

}
