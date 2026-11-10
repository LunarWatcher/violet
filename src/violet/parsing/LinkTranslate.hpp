#pragma once

#include <filesystem>
#include <string>
#include <minilog/minilog.hpp>
#include <sstream>

namespace violet {

extern std::string renameFile(std::string_view sourceFileName);

/**
 * Checks whether or not a path \in in the file \p relSourceFilePath is valid. This is only defined for relative URLs,
 * and must not be called with absolute paths.
 */
extern bool checkPathValidity(
    const std::filesystem::path& relSourceFilePath,
    const std::string& in,
    const std::filesystem::path& projectRoot
);

// TODO: This function has been mocked in a few tests due to one of the arguments previously being the project
// root. This is no longer the case, so we can much more easily use this.
extern std::string projectBasedTranslator(
    const std::string& pathPrefix,
    const std::filesystem::path& relSourceFilePath,
    const std::optional<std::filesystem::path>& projectRoot,
    const std::string& in
);

extern std::string stringifyAndTranslateUrl(
    std::stringstream& url
);


}
