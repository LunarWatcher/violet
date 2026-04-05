#pragma once

#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace violet {

struct Config {
    std::string name;
    std::optional<std::string> description;

    std::optional<std::string> site_prefix;
    std::vector<std::string> exclude;
    nlohmann::json data;
};

extern void from_json(const nlohmann::json& src, Config& dest);

}
