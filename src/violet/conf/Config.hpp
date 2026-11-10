#pragma once

#include "iris/data/DataObject.hpp"
#include "minilog/minilog.hpp"
#include "violet/conf/MarkdownConfig.hpp"
#include "violet/data/Constants.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/parsing/Escaping.hpp"
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace violet {

struct EnvConfig {
    /**
     * Not exposed in JSON: this contains the project root.
     */
    std::filesystem::path root = std::filesystem::current_path();

    bool allowLegacyInjaExtensions = false;
};

// TODO: this entire struct is in desperate need of being rewritten to better reflect what's programmatically used
// vs. what isn't
struct Config {
    std::string name = "";
    std::optional<std::string> description = std::nullopt;

    std::string prefix = "";
    std::vector<std::string> exclude = {};
    std::optional<std::string> theme = "_default";
    iris::Object data = {};

    // Note: This is temporarily static until we can shed nlohmann/json to pass the timezone in directly where
    // appropriate. This being static is a workaround until then
    static inline std::string timezone = "Etc/GMT";

    int tocStartLevel = 2;

    bool metadata = false;

    iris::Object raw = {};

    MarkdownConfig markdown = {};

    std::string destination = "pages";

    /**
     * Not exposed via JSON, but stored internally.  
     */
    EnvConfig env = {};

    const std::filesystem::path& root() const {
        return env.root;
    }

    std::filesystem::path output() const {
        return std::filesystem::weakly_canonical(
            env.root / destination
        );
    }

    Config& merge(const GenerateOpts& opts) {
        this->env.allowLegacyInjaExtensions = opts.allowLegacyInjaExtensions;
        this->env.root = std::filesystem::canonical(opts.root);
        raw[violet::constants::InternalPathRef] = env.root.string();

        // Handle alternate prefixes
        if (opts.overridePrefixForLocalUse) {
            const auto realOutputPath = output();
            minilog::info(
                "Overriding site prefix for local use to {}",
                realOutputPath.string()
            );
            setPrefix(realOutputPath.string());
        } else if (opts.overridePrefix) {
            minilog::info(
                "User requested site prefix override to {}",
                *opts.overridePrefix
            );
            setPrefix(*opts.overridePrefix);
        }
        return *this;
    }

    void setPrefix(const std::string& newPrefix) {
        if (newPrefix == "/") {
            prefix = "";
        } else if (!newPrefix.empty() && !newPrefix.starts_with('/')) {
            minilog::error(
                "The prefix must start with a /, or be an empty string, but found \"{}\"",
                newPrefix
            );
            throw std::runtime_error("Illegal prefix");
        } else if (!newPrefix.empty() && newPrefix.ends_with('/')) {
            minilog::error(
                "The prefix must not end with a / (unless the entire prefix is \"/\"), but found \"{}\"",
                newPrefix
            );
            throw std::runtime_error("Illegal prefix");
        } else if (newPrefix.contains("//")) {
            minilog::error("// is not allowed in the prefix");
            throw std::runtime_error("Illegal prefix");
        } else if (newPrefix.contains("/../") || newPrefix.ends_with("/..")) {
            minilog::error("Path traversal is not allowed in the prefix");
            throw std::runtime_error("Illegal prefix");
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

/**
 * Converts a Config object to JSON.
 *
 * \note This is only intended for creating config programmatically. For passing a loaded Config to a template, use
 *      `config.raw` instead. This is a cached version of the JSON so it doesn't need to be continuously regenerated and
 *      reallocated.
 */
extern void to_json(nlohmann::json& dest, const Config& src);

}
