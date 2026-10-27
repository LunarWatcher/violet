#pragma once

#include <exception>
#include <string>
#include <stacktrace>

namespace violet {

class FatalError : public std::exception {
private:
    const std::string message;
public:
    FatalError(
        std::string&& message,
        const std::stacktrace& trace = std::stacktrace::current()
    ) : message(
        std::format(
            "An irrecoverable internal error occurred: {}\n"
            "Source location for developers: {}\n"
            "This is a bug. Please copy this error message and open an issue on codeberg.",
            std::move(message),
            trace
        )
    ) {}

    const char* what() const noexcept override { return message.c_str(); }
};

}
