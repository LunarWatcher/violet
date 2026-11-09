#pragma once

#include <nlohmann/json.hpp>

namespace violet {

struct MarkdownConfig {
    bool footnoteHeaders = true;
};

void from_json(const nlohmann::json& dest, MarkdownConfig& src);
void to_json(nlohmann::json& dest, const MarkdownConfig& src);

}
