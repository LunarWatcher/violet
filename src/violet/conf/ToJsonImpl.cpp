#include "iris/data/DataObject.hpp"
#include "violet/conf/Config.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/conf/ThemeConfig.hpp"
#include "violet/data/Constants.hpp"
#include "violet/generate/ProcessedFileType.hpp"
#include "violet/parsing/DateTimeParsing.hpp"
#include "violet/transitional/JsonToDataObject.hpp"

void violet::from_json(const nlohmann::json& src, Config& dest) {
    src.at("name").get_to(dest.name);
    if (auto it = src.find("description"); it != src.end() && !it->is_null()) {
        dest.description = it->get<std::string>();
    }
    dest.setPrefix(
        src.value("prefix", "")
    );
    if (auto it = src.find("exclude"); it != src.end()) {
        it->get_to(dest.exclude);
    }

    if (auto it = src.find("data"); it != src.end() && !it->is_null()) {
        if (!it->is_object()) {
            throw std::runtime_error(
                std::format(
                    "illegal data object: expected object, found {}",
                    it->dump()
                )
            );
        }
        dest.data = transitional::convertJson(*it).get<iris::Object>();
    }
    if (auto it = src.find("theme"); it != src.end()) {
        if (!it->is_null()) {
            dest.theme = it->get<std::string>();
        }
    } else {
        dest.theme = "_default";
    }

    if (auto it = src.find("metadata"); it != src.end() && !it->is_null()) {
        dest.metadata = it->get<bool>();
    }

    if (auto it = src.find("timezone"); it != src.end() && !it->is_null()) {
        dest.timezone = it->get<std::string>();

        // This throws an std::runtime_error if the timezone is invalid
        std::chrono::zoned_time testTimezone(
            dest.timezone,
            std::chrono::system_clock::now()
        );
        // The timezone database has links in it, so get_time_zone()->name() returns the name AFTER following the
        // links. This is why the log message is so verbose - Europe/Oslo resolves to Europe/Berlin because it's the
        // same timezone. This will be the case with many (all?) CET timezones. Even the UTC timezone is a link (to
        // Etc/UTC).
        // Also, why is Etc/UTC and Etc/GMT two different timezones? It should be the same timezone? Doesn't really
        // matter though
        minilog::debug(
            "Resolved timezone to {} (note: this may differ from your provided timezone, but this is decided by "
            "your system's timezone database)",
            testTimezone.get_time_zone()->name()
        );
    }

    if (auto it = src.find("toc-start-level"); it != src.end() && !it->is_null()) {
        dest.tocStartLevel = it->get<int>();
        if (dest.tocStartLevel < 1 || dest.tocStartLevel > 6) {
            throw std::runtime_error(
                "Markdown headers only allow h1-h6, so toc-start-level must be 1, 2, 3, 4, 5, or 6. "
                "If you want to disable table of content generation, simply don't include `{{ page.toc }}` in your "
                "template."
            );
        }
    }

    // We manually assemble `raw` to force custom fields into `data`.
    dest.raw = iris::Object {
        {"name", iris::DataObject(dest.name)},
        {"data", iris::DataObject(dest.data)},
        {"exclude", iris::DataObject(dest.exclude)}
    };

    if (dest.description) {
        dest.raw["description"] = dest.description.value();
    }

    dest.raw["prefix"] = dest.prefix;

    if (dest.theme) {
        dest.raw["theme"] = dest.theme.value();
    }

}

void violet::from_json(const nlohmann::json& src, Frontmatter& dest) {
    if (auto it = src.find("title"); it != src.end() && !it->is_null()) {
        dest.title = it->get<std::string>();
    }

    dest.type = src.value("type", "_default");
    if (auto it = src.find("layout"); it != src.end() && !it->is_null()) {
        dest.layout = src.at("layout").get<std::string>();
    }

    if (auto it = src.find("hidden"); it != src.end() && !it->is_null()) {
        dest.hidden = src.at("hidden").get<bool>();
    }

    if (auto it = src.find("date"); it != src.end() && !it->is_null()) {
        if (it->is_string()) {
            auto val = it->get<std::string>();
            dest.date = parseISO8601(val);
        } else {
            dest.date = it->get<int64_t>();
        }
    }
    if (auto it = src.find("last_modified"); it != src.end() && !it->is_null()) {
        if (it->is_string()) {
            auto val = it->get<std::string>();
            dest.last_modified = parseISO8601(val);
        } else {
            dest.last_modified = it->get<int64_t>();
        }
    }

    if (auto it = src.find("taxonomies"); it != src.end() && !it->is_null()) {
        dest.taxonomies = it->get<decltype(dest.taxonomies)>();
    }

    if (auto it = src.find("listing"); it != src.end() && !it->is_null()) {
        if (dest.getLayout() != "page_list") {
            throw std::runtime_error("listing supplied when layout != page_list");
        }
        dest.listing = it->get<ListingFrontmatter>();
    }

    if (auto it = src.find("data"); it != src.end()) {
        if (!it->is_object()) {
            throw std::runtime_error(
                std::format(
                    "illegal data object: expected object, found {}",
                    it->dump()
                )
            );
        }
        dest.data = transitional::convertJson(*it).get<iris::Object>();
    }
    // TODO: not sure if we want this
    if (auto it = src.find(violet::constants::InternalPathRef); it != src.end()) {
        dest.internalPath = *it;
    }
    if (auto it = src.find("summary"); it != src.end() && !it->is_null()) {
        dest.summary = it->get<std::string>();
    }
    if (auto it = src.find(constants::InternalFileTypeRef); it != src.end() && it->is_number()) {
        dest.internalFileType = static_cast<ProcessedFileType>(it->get<int>());

        auto val = static_cast<int>(dest.internalFileType);
        if (val < static_cast<int>(ProcessedFileType::Uninitialized)
            || val > static_cast<int>(ProcessedFileType::InFile)
        ) {
            throw std::runtime_error(
                "Illegal internal filetype found. Have you been tampering with frontmatter objects?"
            );
        }
    }
}

void violet::from_json(const nlohmann::json& src, ListingFrontmatter& dest) {
    dest.recursive = src.value("recursive", false);
    dest.pageSize = src.value("page_size", 50);
    if (dest.pageSize == 0) {
        // TODO: should have a separate exception type for these
        throw std::runtime_error("pageSize must be at least 1");
    }
}

void violet::to_json(nlohmann::json& dest, const Frontmatter& src) {
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access): it's forced true by the time this is called.
    dest["title"] = *src.title;
    dest["type"] = src.type;
    dest["layout"] = src.getLayout();
    if (src.date) {
        dest["date"] = src.date;
    }
    if (src.last_modified) {
        dest["last_modified"] = src.last_modified;
    }

    dest["taxonomies"] = src.taxonomies;
    dest["hidden"] = src.hidden;
    dest["data"] = transitional::convertDataObject(src.data);
    dest["url"] = src.internalUrl;
    dest["table_of_contents"] = src.tableOfContents;
    if (src.summary) {
        dest["summary"] = src.summary;
    }
    dest[violet::constants::InternalPathRef] = src.internalPath;
    dest[violet::constants::InternalFileTypeRef] = static_cast<int>(src.internalFileType);
}

void violet::from_json(const nlohmann::json& src, ThemeConfig& dest) {
    dest.data = transitional::convertJson(
        src.value(
            "data",
            nlohmann::json::object()
        )
    ).get<iris::Object>();
    dest.mount = src.value("mount", std::vector<std::string>());

    dest.raw = iris::Object {
        {"data", dest.data},
        {"mount", dest.mount},
    };
}

void violet::to_json(nlohmann::json& dest, const Config& src) {
    dest["name"] = src.name;
    if (src.description) {
        dest["description"] = *src.description;
    }

    dest["prefix"] = src.prefix;
    dest["exclude"] = src.exclude;
    dest["toc-start-level"] = src.tocStartLevel;
    dest["timezone"] = src.timezone;

    if (src.theme) {
        dest["theme"] = *src.theme;
    }

    dest["data"] = transitional::convertDataObject(
        src.data
    );
}
