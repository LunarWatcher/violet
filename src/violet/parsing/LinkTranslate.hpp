#pragma once

#include <filesystem>
#include <string>
#include <minilog/minilog.hpp>
#include <sstream>

namespace violet {

extern std::string renameFile(const std::string& sourceFileName);

// TODO: This function has been mocked in a few tests due to one of the arguments previously being the project
// root. This is no longer the case, so we can much more easily use this.
extern std::string projectBasedTranslator(
    const std::string& pathPrefix,
    const std::filesystem::path& relSourceFilePath,
    const std::string& in
);

extern std::string stringifyAndTranslateUrl(
    std::stringstream& url
);


}
