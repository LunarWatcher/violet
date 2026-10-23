#pragma once

#include <exception>
#include <filesystem>
#include <optional>
#include <string>
namespace violet {

/**
 * Generic class for parsing exceptions that come before markdown parsing and/or template engine parsing, notably in
 * parsing the frontmatter
 */
class FileParsingException : public std::exception {
private:
    std::string message;
public:
    /**
     * The functions that throw this exception may not know what file the exception is attached to. This is the closest
     * we can get without passing the path of the file into every function - which isn't currently possible as long as
     * we use nlohmann/json anyway.
     * This is filled in after the fact at certain key points. There shouldn't be any paths for this exception that
     * doesn't lead to the file being filled out.
     */
    std::optional<std::filesystem::path> file;

    FileParsingException(std::string&& message) : message(std::move(message)) {}

    const char * what() const noexcept override { return message.c_str(); }
};

}
