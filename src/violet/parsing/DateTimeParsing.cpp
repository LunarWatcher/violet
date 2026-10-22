#include "DateTimeParsing.hpp"

#include <chrono>
#include <sstream>

int64_t violet::parseISO8601(const std::string& raw) {
    std::stringstream ss;
    ss << raw;

    // ok chrono. std::string works, const char* works, but std::string_view is an error
    const char* format;

    // This is a nasty hack to avoid having to pre-parse the date for identification 
    // 8 chars (not parsed): Time only with no timezone
    // 9 chars (not parsed): time only with Z
    // 10 chars: date only
    // 14 chars (not parsed): Time with full timezone
    // 19 chars: datetime with unspecified timezone
    // 20 chars: datetime with Z
    // 25 chars: datetime with full timezone
    if (raw.size() == 10) {
        format = "%F";
    } else if (raw.size() == 19) {
        format = "%FT%T";
    } else if (raw.size() == 20) {
        format = "%FT%T%Z";
    } else if (raw.size() == 25) {
        format = "%FT%T%z";
    } else {
        throw std::runtime_error(
            std::format(
                "Illegal datetime format. Got \"{}\", expected one of the date/datetime ISO-8601 formats. "
                "See https://en.wikipedia.org/wiki/ISO_8601",
                raw
            )
        );
    }

    std::chrono::system_clock::time_point timestamp;

    if (!(ss >> std::chrono::parse(format, timestamp))) {
        throw std::runtime_error(
            std::format(
                "Failed to parse date \"{}\" with format {}",
                timestamp, format
            )
        );
    }

    return std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch())
        .count();
}
