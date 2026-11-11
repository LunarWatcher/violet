#include "Config.hpp"

namespace violet {

Config::Config(const ConfigFileData& config) : confFileData(config) {}

const std::filesystem::path& Config::root() const {
    return env.root;
}

std::filesystem::path Config::output() const {
    return std::filesystem::weakly_canonical(env.root / confFileData.destination);
}

Config& Config::merge(const GenerateOpts& opts) {
    this->env.allowLegacyInjaExtensions = opts.allowLegacyInjaExtensions;
    this->env.root = std::filesystem::canonical(opts.root);
    confFileData.raw[violet::constants::InternalPathRef] = env.root.string();

    // Handle alternate prefixes
    if (opts.overridePrefixForLocalUse) {
        const auto realOutputPath = output();
        minilog::info("Overriding site prefix for local use to {}", realOutputPath.string());
        setPrefix(realOutputPath.string());
    } else if (opts.overridePrefix) {
        minilog::info("User requested site prefix override to {}", *opts.overridePrefix);
        setPrefix(*opts.overridePrefix);
    }

    if (opts.overrideOutputFolder) {
        this->confFileData.destination = *opts.overrideOutputFolder;
    }
    return *this;
}

void Config::setPrefix(const std::string& newPrefix) {
    if (newPrefix == "/") {
        confFileData.prefix = "";
    } else if (!newPrefix.empty() && !newPrefix.starts_with('/')) {
        minilog::error("The prefix must start with a /, or be an empty string, but found \"{}\"", newPrefix);
        throw std::runtime_error("Illegal prefix");
    } else if (!newPrefix.empty() && newPrefix.ends_with('/')) {
        minilog::error(
            "The prefix must not end with a / (unless the entire prefix is \"/\"), but found \"{}\"", newPrefix
        );
        throw std::runtime_error("Illegal prefix");
    } else if (newPrefix.contains("//")) {
        minilog::error("// is not allowed in the prefix");
        throw std::runtime_error("Illegal prefix");
    } else if (newPrefix.contains("/../") || newPrefix.ends_with("/..")) {
        minilog::error("Path traversal is not allowed in the prefix");
        throw std::runtime_error("Illegal prefix");
    } else {
        confFileData.prefix = newPrefix;
    }

    std::stringstream ss;
    // TODO: false, false? Not sure if I want includeReserved set to true here, since # in a
    urlEncode(confFileData.prefix, ss, true, false);
    confFileData.prefix = ss.str();

    confFileData.raw["prefix"] = confFileData.prefix;
}

bool Config::allowLegacyInjaExtensions() const {
    return env.allowLegacyInjaExtensions;
}

int Config::getToCStartLevel() const {
    return confFileData.tocStartLevel;
}

const iris::Object& Config::asIrisObject() const {
    return confFileData.raw;
}

const MarkdownConfig& Config::getMarkdownConfig() const {
    return confFileData.markdown;
}

const std::string& Config::getDestination() const {
    return confFileData.destination;
}

bool Config::getGenerateMetadata() const {
    return confFileData.metadata;
}

const std::string& Config::getTimezone() const {
    return confFileData.timezone;
}

const std::optional<std::string>& Config::getTheme() const {
    return confFileData.theme;
}

const std::vector<std::string>& Config::getExcludes() const {
    return confFileData.exclude;
}

const std::string& Config::getPrefix() const {
    return confFileData.prefix;
}

const std::string& Config::getName() const {
    return confFileData.name;
}

const std::optional<std::string> Config::getDescription() const {
    return confFileData.description;
}

}
