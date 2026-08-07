#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace violet {

struct RenderResult {
    std::string content;
};

extern void to_json(nlohmann::json& dest, const RenderResult& src);

}
