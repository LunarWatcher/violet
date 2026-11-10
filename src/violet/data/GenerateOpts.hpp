#pragma once

#include <filesystem>
#include <string>
#include <optional>

namespace violet {

struct GenerateOpts {
    /**
     * Whether or not to watch for changes.
     */
    bool watch = false;

    bool overridePrefixForLocalUse = false;

    /**
     * Temporary, only for use until v1 or some other point when we can drop the .inja extension outright.  
     */
    bool allowLegacyInjaExtensions = false;

    /**
     * The path to output into
     *
     * If the the folders or intermediate folders do not exist, they will be created on the fly.
     */
    std::optional<std::string> overrideOutputFolder = std::nullopt;
    std::filesystem::path root = std::filesystem::current_path();
    std::optional<std::string> overridePrefix = std::nullopt;
};

}
