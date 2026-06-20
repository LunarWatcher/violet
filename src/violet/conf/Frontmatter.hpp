#pragma once

#include "minilog/minilog.hpp"
#include "violet/parsing/LinkTranslate.hpp"
#include <filesystem>

#include <nlohmann/json.hpp>
#include <unordered_set>

namespace violet {

struct ListingFrontmatter {
    std::optional<std::string> rss;
    bool recursive;
    uint64_t pageSize;
};

struct Frontmatter {
    std::optional<std::string> title;
    std::string type;
    std::string layout;

    std::optional<int64_t> date;
    std::optional<int64_t> last_modified;

    std::optional<ListingFrontmatter> listing;
    std::optional<std::string> summary;

    std::unordered_map<std::string, std::unordered_set<std::string>> taxonomies;

    nlohmann::json data;

    std::string tableOfContents;

    std::string internalUrl;
    std::string internalPath;

    void withFilePath(
        const std::filesystem::path& source
    ) {
        if (!title) {
            title = source.filename().replace_extension();
        }

        internalPath = source.string();
        std::filesystem::path newPath = source;
        if (newPath.filename() != "README.md") {
            newPath = newPath.replace_extension(".html");
            newPath = newPath.replace_filename(
                renameFile(
                    newPath.filename().string()
                )
            );
        } else {
            newPath = newPath.replace_filename("index.html");
        }
        internalUrl = newPath;
        minilog::debug(
            "Page (path: {}, title: {}) has been assigned output URL {}",
            internalPath,
            *title,
            internalUrl
        );
    }
};

extern void from_json(const nlohmann::json& src, Frontmatter& dest);
extern void from_json(const nlohmann::json& src, ListingFrontmatter& dest);

extern void to_json(nlohmann::json& dest, const Frontmatter& src);
extern void to_json(nlohmann::json& dest, const ListingFrontmatter& src);

}
