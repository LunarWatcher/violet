#pragma once

#include <exception>
#include <string>

namespace violet {

class SyntaxError : public std::exception {
private:
    std::string msg;
    size_t offset;
public:
    SyntaxError(
        std::string&& msg,
        size_t offset
    ) : msg(std::format(
            "ERROR: {}\nNear idx:\n{}",
            msg,
            offset
        )),
        offset(offset) {}

    const char * what() const noexcept override {
        return msg.c_str();
    }
};

}
