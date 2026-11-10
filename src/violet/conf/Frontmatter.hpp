#pragma once

#include "iris/data/DataObject.hpp"
#include "minilog/minilog.hpp"
#include "violet/generate/ProcessedFileType.hpp"
#include "violet/parsing/LinkTranslate.hpp"
#include "violet/path/path.hpp"
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
    std::optional<std::string> title = std::nullopt;
    std::string type = "_default";
    std::optional<std::string> layout = std::nullopt;

    std::optional<int64_t> date = std::nullopt;
    std::optional<int64_t> last_modified = std::nullopt;

    std::optional<ListingFrontmatter> listing = std::nullopt;
    std::optional<std::string> summary = std::nullopt;

    bool hidden = false;

    std::unordered_map<std::string, std::unordered_set<std::string>> taxonomies = {};

    iris::Object data = {};

    std::string tableOfContents = "";

    violet::path internalUrl;
    violet::path internalPath;

    ProcessedFileType internalFileType = ProcessedFileType::Uninitialized;
    bool isAsset = false;

    void withFilePath(
        const std::filesystem::path& source
    ) {
        internalPath = source.string();
        internalUrl = source.string();

        auto ext = source.extension().string();
        if (ext == ".js" || ext == ".mjs" || ext == ".css") {
            isAsset = true;
            if (!layout) {
                layout = "none";
            }
            internalFileType = ProcessedFileType::Asset;
        } else if (ext == ".rss" || ext == ".atom" || ext == ".xml") {
            internalFileType = ProcessedFileType::Xml;
        } else if (ext == ".html") {
            internalFileType = ProcessedFileType::Html;
        } else if (ext == ".md") {
            internalFileType = ProcessedFileType::Markdown;

            std::filesystem::path newPath = source;
            if (newPath.filename() != "README.md") {
                newPath.replace_extension(".html");
                newPath.replace_filename(
                    renameFile(
                        newPath.filename().string()
                    )
                );
            } else {
                newPath.replace_filename("index.html");
            }
            // TODO: avoid strong conversion and std::filesystem::path
            internalUrl = newPath.string();
        } else if (ext == ".in") {
            internalFileType = ProcessedFileType::InFile;
            std::filesystem::path newPath = source;
            newPath.replace_extension();
            if (newPath.string() == "") {
                throw std::runtime_error("`.in` is an illegal filename");
            }
            internalUrl = newPath.string();

            if (!layout) {
                layout = "none";
            }
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
        minilog::debug(
            "Page (path: {}, title: {}) has been assigned output URL {}",
            internalPath.to_string(true),
            *title,
            internalUrl.to_string(true)
        );
    }

    bool hasLayout() const noexcept {
        return !layout.has_value() || *layout != "none";
    }

    std::string getLayout() const noexcept {
        return layout.value_or(
            internalFileType == ProcessedFileType::Xml ? "feed" : "single_page"
        );
    }

    operator iris::DataObject() const;

    static Frontmatter fromDataObject(const iris::Object& src);
};


extern void from_json(const nlohmann::json& src, Frontmatter& dest);
extern void from_json(const nlohmann::json& src, ListingFrontmatter& dest);

extern void to_json(nlohmann::json& dest, const Frontmatter& src);
extern void to_json(nlohmann::json& dest, const ListingFrontmatter& src);

}
