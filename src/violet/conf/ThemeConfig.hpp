#pragma once

#include "iris/data/DataObject.hpp"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace violet {

struct ThemeConfig {
    std::vector<std::string> mount;
    iris::Object data;

    iris::Object raw;
};

extern void from_json(const nlohmann::json& in, ThemeConfig& out);

}
