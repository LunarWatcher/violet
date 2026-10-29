#pragma once

#include <string>
#include <violet/conf/Config.hpp>

namespace tests {

struct WithTimezone {
    const std::string oldTimezone;
    WithTimezone(const std::string& timezone = "Etc/GMT")
        : oldTimezone(violet::Config::timezone)
    {
        violet::Config::timezone = timezone;
    }

    ~WithTimezone() {
        violet::Config::timezone = oldTimezone;
    }
};

}
