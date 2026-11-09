#pragma once
// Note: This file is a candidate for being merged into stc: https://codeberg.org/LunarWatcher/stc
// It must NOT contain includes to other parts of violet.

#include <vector>
#include <string>
#include <sstream>

namespace violet {

/**
 * Custom path extension class.
 *
 * This class treats the provided segment as a root, allowing relative paths that never care about the root directory of
 * a filesystem.
 *
 * If this class is given `some/path` as input, no amount of `../` will bring it outside whatever the application
 * considers its root directory.
 *
 * This class has no concept of absolute or relative paths. It deals with what it is given, and leaves figuring out the
 * rest to the application. This is because this does not necessarily represent a filesystem path, though that is
 * allowed as well; this may represent a URI path, for example.
 */
class path {
private:
    std::vector<std::string> segments;

    void push(const std::string& segment) {
        if (segment == ".") {
            return;
        } else if (segment == "..") {
            if (!segments.empty()) {
                segments.pop_back();
            }
        } else {
            segments.push_back(segment);
        }
    }

    void parse(const std::string& src) {
        size_t idx = 0, nextIdx;
        while (
            (nextIdx = src.find_first_of(
#ifndef _WIN32
                '/',
#else
                // Windows: Search for both / and \. / is reserved in NTFS
                "/\\",
#endif
                idx
            )) != std::string::npos
        ) {
            if (idx != nextIdx) {
                std::string substr = src.substr(idx, nextIdx - idx);
                push(substr);
            }

            idx = nextIdx + 1;
        }

        if (idx < src.size()) {
            auto rest = src.substr(idx);
            push(rest);
        }
    }
public:
    path(const char* v) : path(std::string(v)) {}
    path(const std::string& src) {
        parse(src);
    }
    path(const std::vector<std::string>& src) {
        this->segments = src;
    }
    path(const path&) = default;

    operator std::string() const {
        return to_string();
    }
    std::string to_string(bool mockRelative = false) const {
        std::stringstream ss;

        if (segments.size() == 0) {
            if (!mockRelative) {
                ss << "/";
            }
        } else {
            for (auto& segment : segments) {
                if (!mockRelative || (mockRelative && ss.tellp() > 0)) {
                    ss << "/";
                }
                ss << segment;
            }
        }

        return ss.str();
    }

    std::string concatPrefix(const std::string& prefix) {
        path p {prefix};
        p.join(*this);
        return p;
    }

    path& join(const std::string& value) {
        parse(value);
        return *this;
    }

    path& join(const path& value) {
        for (auto& segment : value.segments) {
            push(segment);
        }
        return *this;
    }

    path parent_path() {
        if (segments.size() == 0) {
            return *this;
        }
        auto segments = this->segments;
        segments.pop_back();
        return path { segments };
    }

    const std::vector<std::string>& getSegments() { return segments; }

    path& operator=(const path& other) {
        this->segments = other.segments;
        return *this;
    }
};


}
