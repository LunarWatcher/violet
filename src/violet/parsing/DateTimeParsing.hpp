#pragma once

#include <cstdint>
#include <string>

namespace violet {

/**
 * Parses two variants of ISO 8601 datetimes, specifically datetimes with offsets and datetime with UTC. Separators are
 * mandatory in this parser because who teh fuck writes UTC datetimes without separators?
 *
 * \see https://en.wikipedia.org/wiki/ISO_8601
 * \returns the number of seconds since the epoch
 */
extern int64_t parseISO8601(const std::string& input);

}
