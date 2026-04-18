#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

namespace violet {

struct ListingFrontmatter {
    std::string rss;
    bool recursive;
};

struct Frontmatter {
    std::optional<std::string> title;
    std::string type;
    std::string layout;

    int64_t date;
    int64_t last_modified;

    nlohmann::json data;

    void withFilePath(
        const std::filesystem::path& source
    ) {
        if (!title) {
            title = source.filename();
        }
    }
};

extern void from_json(const nlohmann::json& src, ListingFrontmatter& dest);
extern void from_json(const nlohmann::json& src, Frontmatter& dest);

}
