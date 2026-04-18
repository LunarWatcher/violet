#include "Config.hpp"

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

}
