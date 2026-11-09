#pragma once

#include <vector>
#include <ostream>
#include <format>
// #include <print>

template <typename T>
inline std::ostream& operator<<(std::ostream& o, const std::vector<T>& src) {
    return o << std::format("{}", src);
}
