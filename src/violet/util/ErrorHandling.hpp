#pragma once

#include "iris/exceptions.hpp"
#include "violet/exceptions/FileParsingException.hpp"
#include "violet/exceptions/SyntaxError.hpp"
#include <filesystem>
#include <sstream>

namespace violet {

extern void renderSyntaxError(
    const std::filesystem::path& relPath,
    const SyntaxError& synError,
    std::stringstream& content
);

extern void renderFileParsingException(
    const FileParsingException& fpError
);

extern void renderIrisError(
    const iris::IrisError& err,
    const std::filesystem::path& relPath
);

}
