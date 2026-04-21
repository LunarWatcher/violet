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

    std::optional<int64_t> date;
    std::optional<int64_t> last_modified;

    std::optional<ListingFrontmatter> listing;

    nlohmann::json data;

    void withFilePath(
        const std::filesystem::path& source
    ) {
        if (!title) {
            title = source.filename().replace_extension();
        }
    }
};

extern void from_json(const nlohmann::json& src, Frontmatter& dest);
extern void from_json(const nlohmann::json& src, ListingFrontmatter& dest);

extern void to_json(nlohmann::json& dest, const Frontmatter& src);
extern void to_json(nlohmann::json& dest, const ListingFrontmatter& src);

}
