#pragma once

#include <sstream>

namespace violet {

void htmlEscape(
    char ch,
    std::stringstream& target
);

/**
 * URL-encodes a character
 *
 * \param includeReserved       Whether or not the function should URL encode reserved characters, meaning characters
 *                              that have a meaning in a URL. If false (default), these are ignored. If true, they're
 *                              URL-encoded. Note that the calling function is responsible for validating whether a
 *                              reserved character does or doesn't have a meaning. Blindly calling this and feeding the
 *                              function a URL one byte at a time means you'd end up with
 *                              `https%3A%2F%2Fexample.com%0A`, which would obviously break the URL. This is good if
 *                              it's passed as a part of a URL, but not if it _is_ the URL.
 *                              This is as defined by RFC 3986 (jan. 2005)
 */
void urlEncode(
    char ch,
    std::stringstream& target,
    bool includeReserved = false
);

}
