#pragma once

#include <exception>
#include <string>

namespace violet {

class TemplateError : public std::exception {
private:
    std::string msg;
public:
    TemplateError(
        std::string&& msg
    ) : msg(std::move(msg)) {}

    const char * what() const noexcept override {
        return msg.c_str();
    }
};

}
