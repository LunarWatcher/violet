#pragma once

#include "violet/conf/Config.hpp"
#include <filesystem>

namespace violet::util {

/**
 * Used to determine common excludes. This should only include code that apples to both files and folders, and applies
 * equally to the server and generate command. Notably, `violet.json` should not be included in this file, as the server
 * needs to know about it.  
 */
extern bool shouldExclude(const std::filesystem::path& path, const Config& cfg);

}
