#pragma once

#include "violet/algorithm/SortMethod.hpp"
#include <nlohmann/json.hpp>

namespace violet {

inline const nlohmann::json violet_global {
    {"datetime", {
            {"iso", "%FT%H:%M:%OS%Oz"}
        }},
    {"sort", {
            {"by_creation_date", static_cast<int>(SortMethod::ByCreationDate)},
            {"by_last_modified_date", static_cast<int>(SortMethod::ByLastModifiedDate)},
        }}
};

}
