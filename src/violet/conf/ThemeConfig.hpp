#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace violet {

struct ThemeConfig {
    std::vector<std::string> mount;
    nlohmann::json data;

    nlohmann::json raw;
};

extern void from_json(const nlohmann::json& in, ThemeConfig& out);

}
