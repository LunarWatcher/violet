#pragma once

#include "minilog/minilog.hpp"
#include "violet/generate/ProcessedFileType.hpp"
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

    ProcessedFileType internalFileType = ProcessedFileType::Uninitialized;
    bool isAsset = false;

    void withFilePath(
        const std::filesystem::path& source
    ) {
        auto ext = source.extension().string();
        if (ext == ".js" || ext == ".mjs" || ext == ".css") {
            isAsset = true;
            layout = "";
            internalFileType = ProcessedFileType::Asset;
        } else if (ext == ".rss" || ext == ".atom" || ext == ".xml") {
            internalFileType = ProcessedFileType::Xml;
        } else if (ext == ".html") {
            internalFileType = ProcessedFileType::Html;
        } else if (ext == ".md") {
            internalFileType = ProcessedFileType::Markdown;
        } else {
            throw std::runtime_error(
                std::format(
                    "Frontmatter attempted initialized on illegal filetype {}",
                    ext
                )
            );
        }
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
