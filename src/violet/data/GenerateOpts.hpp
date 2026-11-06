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

    /**
     * The path to output into
     *
     * If the the folders or intermediate folders do not exist, they will be created on the fly.
     */
    std::string outputFolder = "pages";

    bool overridePrefixForLocalUse = false;

    /**
     * Temporary, only for use until v1 or some other point when we can drop the .iris extension outright.  
     */
    bool allowLegacyInjaExtensions = false;

    std::filesystem::path root = std::filesystem::current_path();
    std::optional<std::string> overridePrefix = std::nullopt;

    void postParse() {
        this->root = std::filesystem::weakly_canonical(
            std::filesystem::absolute(
                this->root
            )
        );
    }

    std::filesystem::path output() {
        return std::filesystem::weakly_canonical(
            root / outputFolder
        );
    }
};

}
