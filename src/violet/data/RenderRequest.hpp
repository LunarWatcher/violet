#pragma once

#include "violet/data/GenerateOpts.hpp"
#include "nlohmann/json.hpp"

namespace violet {

struct RenderRequest {
    // Provides {{ site }}
    const nlohmann::json& siteConfig;
    // Provides {{ page }}
    const nlohmann::json& frontmatter;
    // Provides {{ theme }} (if not null)
    const nlohmann::json& themeConfig;

    // Non-providers; defines where to putput the rendered page
    const std::filesystem::path& outputDir;
    const std::filesystem::path& outputFilename;

    // Provides {{ content }} (TODO: does this allow injected recursion of the inja templates?)
    const std::string& content;
};

}
