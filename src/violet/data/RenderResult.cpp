#include "RenderResult.hpp"

void violet::to_json(nlohmann::json& out, const RenderResult& src) {
    out["content"] = src.content;
    
}
