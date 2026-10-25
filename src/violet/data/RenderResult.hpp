#pragma once

#include "iris/data/DataObject.hpp"
#include <string>
#include <nlohmann/json.hpp>

namespace violet {

struct RenderResult {
    std::string content;

    operator iris::DataObject();
};

}
