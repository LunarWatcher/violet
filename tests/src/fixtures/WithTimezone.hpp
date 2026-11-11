#pragma once

#include <string>
#include <violet/conf/Config.hpp>

namespace tests {

struct WithTimezone {
    const std::string oldTimezone;
    WithTimezone(const std::string& timezone = "Etc/GMT")
        : oldTimezone(violet::ConfigFileData::timezone)
    {
        violet::ConfigFileData::timezone = timezone;
    }

    ~WithTimezone() {
        violet::ConfigFileData::timezone = oldTimezone;
    }
};

}
