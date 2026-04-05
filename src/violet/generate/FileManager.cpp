#include "FileManager.hpp"

namespace violet {

void FileManager::copyRaw(
    const std::filesystem::path& root,
    const std::filesystem::path& relPath,
    const std::filesystem::path& destName
) {
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

}
