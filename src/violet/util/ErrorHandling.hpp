#pragma once

#include "violet/exceptions/SyntaxError.hpp"
#include <filesystem>
#include <sstream>

namespace violet {

extern void renderSyntaxError(
    const std::filesystem::path& relPath,
    const SyntaxError& synError,
    std::stringstream& content
);

}
