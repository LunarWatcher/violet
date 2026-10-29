#include "DateTimeParsing.hpp"

#include <chrono>
#include <sstream>
#include "violet/exceptions/FileParsingException.hpp"
#include "violet/conf/Config.hpp"
#include <format>

int64_t violet::parseDate(
    const std::string& raw,
    const std::string& format
) {
    std::stringstream ss;
    ss << raw;

    if (format.contains("%z") || format.contains("%Z")) {
        std::chrono::system_clock::time_point timestamp;
        if (!(ss >> std::chrono::parse(format, timestamp))) {
            throw FileParsingException(
                std::format(
                    "Failed to parse date \"{}\" with format {}",
                    raw, format
                )
            );
        }
        return std::chrono::duration_cast<std::chrono::seconds>(
            timestamp.time_since_epoch()
        ).count();
    } else {
        std::chrono::local_seconds timestamp;

        if (!(ss >> std::chrono::parse(format, timestamp))) {
            throw FileParsingException(
                std::format(
                    "Failed to parse date \"{}\" with format {}",
                    raw, format
                )
            );
        }

        std::chrono::zoned_time t { Config::timezone, timestamp };
        return std::chrono::duration_cast<std::chrono::seconds>(
            t.get_sys_time().time_since_epoch()
        ).count();
    }
}

int64_t violet::parseISO8601(const std::string& raw) {

    std::string format;

    // This is a nasty hack to avoid having to pre-parse the date for identification 
    // 8 chars (not parsed): Time only with no timezone
    // 9 chars (not parsed): time only with Z
    // 10 chars: date only
    // 14 chars (not parsed): Time with full timezone
    // 19 chars: datetime with unspecified timezone
    // 20 chars: datetime with Z
    // 24 chars: same as 25 chars, but without a : between the timezones
    // 25 chars: datetime with full timezone
    if (raw.size() == 10) {
        format = "%F";
    } else if (raw.size() == 19) {
        format = "%FT%T";
    } else if (raw.size() == 20) {
        format = "%FT%T%Z";
    } else if (raw.size() == 24 || raw.size() == 25) {
        format = "%FT%T%z";
    } else {
        throw FileParsingException(
            std::format(
                "Illegal datetime format. Got \"{}\", expected one of the date/datetime ISO-8601 formats. "
                "See https://en.wikipedia.org/wiki/ISO_8601",
                raw
            )
        );
    }

    return parseDate(raw, format);
}

std::string violet::formatDate(int64_t ts, const std::string& fmt, const std::string& tz) {
    auto tp = std::chrono::zoned_time(
        tz,
        std::chrono::system_clock::time_point(
            std::chrono::seconds(ts)
        )
    );
    return std::vformat(
        "{:" + fmt + "}",
        std::make_format_args(
            tp
        )
    );
}
