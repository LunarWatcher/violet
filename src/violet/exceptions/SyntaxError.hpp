#pragma once

#include <exception>
#include <string>
#include <format>

namespace violet {

class SyntaxError : public std::exception {
private:
    std::string msg;
public:
    const size_t offset;
    SyntaxError(
        std::string&& msg,
        size_t offset
    ) : msg(std::format(
            "{}\nNear byte:\n{}",
            msg,
            offset
        )),
        offset(offset) {}

    const char * what() const noexcept override {
        return msg.c_str();
    }
};

}
