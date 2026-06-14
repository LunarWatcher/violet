#pragma once

#include "inja/function_storage.hpp"
#include <catch2/catch_test_macros.hpp>

namespace tests {

struct InjaArgs {
    nlohmann::json source;

    [[nodiscard("Discarding or directly invoking asArgs causes undefined behaviour")]]
    InjaArgs(
        nlohmann::json&& source
    ) : source(std::move(source)) {
        INFO("Source must be an array");
        REQUIRE(this->source.is_array());
    }

    inja::Arguments asArgs() {
        inja::Arguments args;

        for (auto& it : source) {
            args.push_back(&it);
        }
        return args;
    }
};

}
