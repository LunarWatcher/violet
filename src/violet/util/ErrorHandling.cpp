#include "ErrorHandling.hpp"
#include "minilog/minilog.hpp"
#include <iostream>
#include <sstream>
#include <stc/StringUtil.hpp>

void violet::renderSyntaxError(
    const std::filesystem::path& relPath,
    const SyntaxError& synError,
    std::stringstream& content
) {
    minilog::error("Markdown error in {}", relPath.string());
    minilog::error("Error: {}", synError.what());

    auto pos = synError.offset;
    content.seekg(0);
    std::string asStr = content.str();
    if (asStr.size() == 0 || pos >= asStr.size()) {
        minilog::error("The file is empty!");
        return;
    }
    std::cerr << "Location:" << std::endl;
    std::string_view source = asStr;
    size_t start = pos;
    size_t end = pos;
    if (start > 0) {
        for (size_t i = 0; i < 4; ++i) {
            start = asStr.rfind('\n', start - 1);
            if (start == std::string::npos || start == 0) {
                start = 0;
                break;
            }
        }
    }

    if (end < asStr.size() - 1) {
        for (size_t i = 0; i < 3; ++i) {
            end = asStr.find('\n', end + 1);
            if (end == std::string::npos || end == asStr.size() - 1) {
                end = asStr.size() > 0 ? asStr.size() - 1 : 0;
                break;
            }
        }
    }

    auto normalizedStart = start == 0 ? start : start + 1;

    std::string_view rangeLimitedContent = source.substr(normalizedStart, end - normalizedStart);

    // Temporary string conversion: gonna make upstream support string_view, but not right now
    // TODO: Actually, instead, can we do something with ranges to stream it instead?
    std::vector<std::string> lines = stc::string::split(std::string(rangeLimitedContent), '\n');

    // TODO: This is very much not compatible with DOS endings
    size_t textPtr = normalizedStart;
    for (const auto& line : lines) {
        auto lineEnd = textPtr + line.size() + 1;

        std::cerr << "  " << line << "\n";
        if (pos >= textPtr && pos <= lineEnd) {
            // This is the line
            std::cerr << "  ";
            // Very bad and very naive implementation. Also probably suboptimal code in general
            std::stringstream ss;
            for (size_t i = 0; i < pos - textPtr; ++i) {
                ss << '~';
            }
            std::cerr << ss.rdbuf() << '^' << std::endl;
        }
        // +2: +1 for newline, +1 to actually have the ptr start at the line
        textPtr += line.size() + 2;
    }

}

void violet::renderFileParsingException(
    const FileParsingException& fpError
) {
    minilog::error(
        "Failed to parse file {} with message: {}",
        fpError.file.has_value() ? fpError.file.value().string() : "[Unknown file: this is a bug, please open an issue]",
        fpError.what()
    );
}

