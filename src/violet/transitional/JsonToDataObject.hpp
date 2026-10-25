#pragma once

#include "iris/data/DataObject.hpp"
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace violet::transitional {

inline iris::DataObject convertJson(const nlohmann::json& source) {
    if (source.is_object()) {
        iris::DataObject obj = iris::Object {};
        for (const auto& [k, v] : source.items()) {
            obj[k] = convertJson(v);
        }
        return obj;
    } else if (source.is_array()) {
        iris::DataObject arr = iris::Array {};
        for (const auto& it : source) {
            arr.push_back(convertJson(it));
        }
        return arr;
    } else if (source.is_boolean()) {
        return source.get<bool>();
    } else if (source.is_number_integer()) {
        return source.get<int64_t>();
    } else if (source.is_number_float()) {
        return source.get<double>();
    } else if (source.is_null()) {
        return nullptr;
    } else if (source.is_string()) {
        return source.get<std::string>();
    } else {
        throw std::runtime_error(
            std::format(
                "Error while converting JSON to DataObject: unknown type at {}", source.dump()
            )
        );
    }
}

inline nlohmann::json convertDataObject(const iris::DataObject& source) {
    if (source.is_object()) {
        auto obj = nlohmann::json::object();
        for (const auto& [k, v] : source.get<iris::Object>()) {
            obj[k] = convertDataObject(v);
        }
        return obj;
    } else if (source.is_array()) {
        auto arr = nlohmann::json::array();
        for (const auto& it : source.get<iris::Array>()) {
            arr.push_back(convertDataObject(it));
        }
        return arr;
    } else if (source.is_bool()) {
        return source.get<bool>();
    } else if (source.is_int()) {
        return source.get<iris::Int>();
    } else if (source.is_float()) {
        return source.get<iris::Float>();
    } else if (source.is_null()) {
        return nullptr;
    } else if (source.is_string()) {
        return source.get<std::string>();
    } else {
        throw std::runtime_error(
            std::format(
                "Error while converting DataObject to JSON: unknown type at {}",
                source.serialize()
            )
        );
    }
}

}
