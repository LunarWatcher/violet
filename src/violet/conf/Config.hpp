#pragma once

#include "minilog/minilog.hpp"
#include "violet/data/Constants.hpp"
#include "violet/parsing/Escaping.hpp"
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace violet {

struct Config {
    std::string name;
    std::optional<std::string> description;

    std::string prefix;
    std::vector<std::string> exclude;
    std::optional<std::string> theme;
    nlohmann::json data;

    bool metadata = false;

    /**
     * Contains a copy of the raw JSON object that created the rest of this struct; used for forwarding to inja.
     */
    nlohmann::json raw;

    void imbueRoot(const std::filesystem::path& root) {
        raw[violet::constants::InternalPathRef] = root.string();
    }

    void setPrefix(const std::string& newPrefix) {
        if (newPrefix == "/") {
            prefix = "";
        } else if (!newPrefix.empty() && !newPrefix.starts_with("/")) {
            minilog::error(
                "The prefix must start with a /, or be an empty string, but found \"{}\"",
                newPrefix
            );
            throw std::runtime_error(
                "Illegal prefix"
            );
        } else if (!newPrefix.empty() && newPrefix.ends_with("/")) {
            minilog::error(
                "The prefix must not end with a / (unless the entire prefix is \"/\"), but found \"{}\"",
                newPrefix
            );
            throw std::runtime_error(
                "Illegal prefix"
            );
        } else {
            prefix = newPrefix;
        }


        std::stringstream ss;
        // TODO: false, false? Not sure if I want includeReserved set to true here, since # in a 
        urlEncode(prefix, ss, true, false);
        prefix = ss.str();

        raw["prefix"] = prefix;
    }
};

extern void from_json(const nlohmann::json& src, Config& dest);

}
