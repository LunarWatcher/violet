#pragma once

#include <cstddef>
#include <string_view>

namespace tests {

// TODO: this function is a candidate for inclusion in stc
inline size_t count(const std::string_view& haystack, const std::string_view& needle) {
    size_t pos = 0;
    size_t occurrences = 0;
    while ((pos = haystack.find(needle, pos)) != std::string_view::npos) {
        ++occurrences;
        pos += needle.length();
    }
    return occurrences;
}

}
