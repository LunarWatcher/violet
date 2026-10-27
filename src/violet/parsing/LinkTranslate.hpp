#pragma once

#include <filesystem>
#include <string>
#include <minilog/minilog.hpp>
#include <sstream>

namespace violet {

extern std::string renameFile(const std::string& sourceFileName);

extern std::string projectBasedTranslator(
    const std::string& pathPrefix,
    const std::filesystem::path& projectRoot,
    const std::filesystem::path& relSourceFilePath,
    const std::string& in
);

extern std::string stringifyAndTranslateUrl(
    std::stringstream& url
);


}
