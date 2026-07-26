#pragma once

#include <cstdint>
#include <chrono>
#include <string_view>

#include <nlohmann/json.hpp>

namespace violet::meta {

// TODO: this file is not thread-safe. It should be using `std::atomic` for everything that's incremented.

struct Input {
    uint64_t pages = 0;
    uint64_t files = 0;
};

struct Output {
    uint64_t pages = 0;
};

struct Build {
    uint64_t buildMillis;
    const std::chrono::system_clock::time_point startTimestamp = std::chrono::system_clock::now();
    // This is a const, but it makes sense to include it in the struct for readability wrt. the serialization
    // process. Otherwise, this would jsut be hidden in MetadataJson.cpp
    const static inline std::string_view version = VIOLET_VERSION;

    void commitBuildMillis();
};

struct MetadataJson {
    Input input;
    Output output;
    Build build;
};

void to_json(nlohmann::json& out, const Input& in);
void to_json(nlohmann::json& out, const Output& in);
void to_json(nlohmann::json& out, const Build& in);
void to_json(nlohmann::json& out, const MetadataJson& in);

// Only used for tests
void from_json(const nlohmann::json& in, Input& out);
void from_json(const nlohmann::json& in, Output& out);
void from_json(const nlohmann::json& in, Build& out);
void from_json(const nlohmann::json& in, MetadataJson& out);

}
