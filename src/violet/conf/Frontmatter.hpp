#pragma once

#include <chrono>
#include <filesystem>
#include <string>

#include <nlohmann/json.hpp>

namespace violet {

struct Frontmatter {
    std::string title;
    std::string type;
    std::chrono::zoned_seconds date;
    std::chrono::zoned_seconds last_modified;

    nlohmann::json rawWithIntDates;

    void loadFileData(
        std::filesystem::path& source
    ) {
        if (title == "") {
            title = source.filename();
        }
    }
};

void from_json(const nlohmann::json& src, con)

}
