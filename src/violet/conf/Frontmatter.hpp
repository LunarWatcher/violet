#pragma once

#include <chrono>
#include <filesystem>
#include <string>

#include <nlohmann/json.hpp>

namespace violet {

namespace Frontmatter {
    inline void loadFileData(
        nlohmann::json& src,
        const std::filesystem::path& source
    ) {
        auto title = src.find("title");
        if (title == src.end() || *title == "") {
            *title = source.filename();
        }
    }
}

}
