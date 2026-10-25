#pragma once

#include "iris/data/DataObject.hpp"
#include "iris/function_storage.hpp"
#include <catch2/catch_test_macros.hpp>

namespace tests {

struct IrisArgs {
    iris::Array source;

    [[nodiscard("Discarding or directly invoking asArgs causes undefined behaviour")]]
    IrisArgs(
        iris::Array&& source
    ) : source(std::move(source)) {}

    iris::Arguments asArgs() {
        iris::Arguments args;

        for (auto& it : source) {
            args.push_back(&it);
        }
        return args;
    }
};

}
