#include "DateTimeParsing.hpp"

#include <chrono>
#include <sstream>

int64_t violet::parseISO8601(const std::string& raw) {
    std::stringstream ss;
    ss << raw;

    // ok chrono. std::string works, const char* works, but std::string_view is an error
    const char* format;
    if (raw.contains("Z")) {
        format = "%FT%T%Z";
    } else {
        format = "%FT%T%z";
    }

    std::chrono::system_clock::time_point timestamp;

    ss >> std::chrono::parse(format, timestamp);

    return std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch())
        .count();
}
