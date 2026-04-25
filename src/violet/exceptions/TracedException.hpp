#pragma once

#include <exception>
#include <string>
#include <stacktrace>

namespace violet {

class TracedException : public std::exception {
private:
    const std::string message;
public:
    TracedException(
        std::string&& message,
        const std::stacktrace& trace = std::stacktrace::current()
    ) : message(
        std::format(
            "{}\n{}",
            std::move(message),
            trace
        )
    ) {}

    const char* what() const noexcept override { return message.c_str(); }
};

}
