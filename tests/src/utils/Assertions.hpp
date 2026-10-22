#pragma once

#define REQUIRE_VALUE(expr) [&]() {              \
        auto opt = expr;                        \
        REQUIRE(opt.has_value());               \
        return *opt;                            \
    }()
