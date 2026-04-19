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
    std::optional<std::string> theme;
    nlohmann::json data;

    /**
     * Contains a copy of the raw JSON object that created the rest of this struct; used for forwarding to inja.
     */
    nlohmann::json raw;
};

extern void from_json(const nlohmann::json& src, Config& dest);

}
