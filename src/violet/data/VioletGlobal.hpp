#pragma once

#include "iris/data/DataObject.hpp"
#include "violet/algorithm/SortMethod.hpp"
#include "violet/data/GitVersion.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace violet {

inline const std::string DateFormatISO = "%FT%H:%M:%OS%Oz";

inline const iris::DataObject violet_global = iris::Object {
    {"datetime", iris::Object {
        {"iso", DateFormatISO },
    }},
    {"sort", iris::Object {
        {"by_creation_date", static_cast<int>(SortMethod::ByCreationDate)},
        {"by_last_modified_date", static_cast<int>(SortMethod::ByLastModifiedDate)},
    }},
    {"version", std::string(GitVersion())},
};

}
