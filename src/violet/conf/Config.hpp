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

// TODO: This is a shit name, but I don't have any better suggestions
struct ConfigFileData {
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
};

class Config {
private:
    ConfigFileData confFileData;

    EnvConfig env = {};
public:
    Config() = default;
    Config(const ConfigFileData& config);

    const std::filesystem::path& root() const;

    std::filesystem::path output() const;

    Config& merge(const GenerateOpts& opts);

    void setPrefix(const std::string& newPrefix);

    bool allowLegacyInjaExtensions() const;

    int getToCStartLevel() const;

    const iris::Object& asIrisObject() const;

    const MarkdownConfig& getMarkdownConfig() const;

    const std::string& getDestination() const;

    bool getGenerateMetadata() const;

    /**
     * This function should be used over `ConfigFileData::timezone` anywhere that's physically capable of getting a
     * Config instance.
     */
    const std::string& getTimezone() const;

    const std::optional<std::string>& getTheme() const;

    const std::vector<std::string>& getExcludes() const;

    const std::string& getPrefix() const;

    const std::string& getName() const;

    const std::optional<std::string> getDescription() const;

    friend void from_json(const nlohmann::json& src, Config& dest);
    friend void to_json(nlohmann::json& dest, const Config& src);
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
