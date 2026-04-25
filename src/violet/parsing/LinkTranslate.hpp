#pragma once

#include <string>
#include <sstream>

namespace violet {

inline std::string stringifyAndTranslateUrl(
    std::stringstream& url
) {
    std::string out = url.str();

    // TODO: would be cleaner to do this with stream manipulation, but not sure how
    // This is also not perfect, but it's good enough for now
    if (
        !out.starts_with("http://")
        && !out.starts_with("https://")
        && (
            !out.starts_with("/")
            || (out.starts_with("/") && !out.starts_with("//"))
        )
        && out.ends_with(".md")
    ) {
        auto sub = out.substr(0, out.size() - 3);
        return sub + ".html";
    }
    return out;
}


}
