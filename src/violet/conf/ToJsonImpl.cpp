#include "violet/conf/Config.hpp"
#include "violet/conf/Frontmatter.hpp"
#include "violet/conf/ThemeConfig.hpp"
#include "violet/parsing/DateTimeParsing.hpp"

void violet::from_json(const nlohmann::json& src, Config& dest) {
    src.at("name").get_to(dest.name);
    if (auto it = src.find("description"); it != src.end() && !it->is_null()) {
        dest.description = it->get<std::string>();
    }
    if (auto it = src.find("site_prefix"); it != src.end() && !it->is_null()) {
        dest.site_prefix = it->get<std::string>();
    }
    if (auto it = src.find("exclude"); it != src.end()) {
        it->get_to(dest.exclude);
    }

    if (auto it = src.find("data"); it != src.end() && !it->is_null()) {
        it->get_to(dest.data);
    }
    if (auto it = src.find("theme"); it != src.end()) {
        if (!it->is_null()) {
            dest.theme = it->get<std::string>();
        }
    } else {
        dest.theme = "_default";
    }

    // We manually assemble `raw` to force custom fields into `data`.
    dest.raw = {
        {"name", dest.name},
        {"data", dest.data},
        {"exclude", dest.exclude}
    };

    if (dest.description) {
        dest.raw["description"] = dest.description.value();
    }

    if (dest.site_prefix) {
        dest.raw["site_prefix"] = dest.site_prefix.value();
    }

    if (dest.theme) {
        dest.raw["theme"] = dest.theme.value();
    }

}

void violet::from_json(const nlohmann::json& src, Frontmatter& dest) {
    if (auto it = src.find("title"); it != src.end() && !it->is_null()) {
        dest.title = it->get<std::string>();
    }

    dest.type = src.value("type", "_default");
    dest.layout = src.value("layout", "single_page");

    if (auto it = src.find("date"); it != src.end() && !it->is_null()) {
        auto val = it->get<std::string>();
        dest.date = parseISO8601(val);
    }
    if (auto it = src.find("last_modified"); it != src.end() && !it->is_null()) {
        auto val = it->get<std::string>();
        dest.date = parseISO8601(val);
    }

    if (auto it = src.find("taxonomies"); it != src.end() && !it->is_null()) {
        dest.taxonomies = it->get<decltype(dest.taxonomies)>();
    }

    if (auto it = src.find("listing"); it != src.end() && !it->is_null()) {
        if (dest.layout != "page_list") {
            throw std::runtime_error("listing supplied when layout != page_list");
        }
        dest.listing = it->get<ListingFrontmatter>();
    }

    if (auto it = src.find("data"); it != src.end()) {
        dest.data = *it;
    }
    // TODO: not sure if we want this
    if (auto it = src.find("violet_internals.path"); it != src.end()) {
        dest.internalPath = *it;
    }
    if (auto it = src.find("summary"); it != src.end() && !it->is_null()) {
        dest.summary = it->get<std::string>();
    }

}

void violet::from_json(const nlohmann::json& src, ListingFrontmatter& dest) {
    if (auto it = src.find("rss"); it != src.end() && !it->is_null()) {
        dest.rss = it->get<std::string>();
    }

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
    dest["layout"] = src.layout;
    if (src.date) {
        dest["date"] = src.date;
    }
    if (src.last_modified) {
        dest["last_modified"] = src.last_modified;
    }

    if (src.listing) {
        dest["listing"] = src.listing;
    }

    dest["taxonomies"] = src.taxonomies;

    dest["data"] = src.data;
    dest["violet_internals.path"] = src.internalPath;
    dest["url"] = src.internalUrl;
    dest["table_of_contents"] = src.tableOfContents;
    if (src.summary) {
        dest["summary"] = src.summary;
    }
}

// TODO: isn't this internal data that we don't want to re-expose again?
void violet::to_json(nlohmann::json& dest, const ListingFrontmatter& src) {
    if (src.rss) {
        dest["rss"] = src.rss;
    }
    dest["recursive"] = src.recursive;
    dest["page_size"] = src.pageSize;
}

void violet::from_json(const nlohmann::json& src, ThemeConfig& dest) {
    dest.data = src.value(
        "data",
        nlohmann::json::object()
    );
    dest.mount = src.value("mount", std::vector<std::string>());

    dest.raw = {
        {"data", dest.data},
        {"mount", dest.mount},
    };
}
