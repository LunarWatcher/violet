#pragma once

#include "violet/parsing/Escaping.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace violet {

inline std::string renameFile(const std::string& sourceFileName) {
    std::stringstream ss;
    for (auto& ch : sourceFileName) {
        if (
            (ch >= 'a' && ch <= 'z')
            || (ch >= '0' && ch <= '9')
            || ch == '-'
            || ch == '_'
            || ch == '.'
        ) {
            ss << ch;
        } else if (
            (ch >= 'A' && ch <= 'Z')
        ) {
            ss << (char) std::tolower(ch);
        } else if (ch == ' ') {
            ss << "-";
        }
    }

    if (ss.tellp() == 0) {
        throw std::runtime_error(
            std::format(
                "File name {} contains no latin letters, and cannot be translated",
                sourceFileName
            )
        );
    }
    return ss.str();
}

inline std::string projectBasedTranslator(
    const std::string& pathPrefix,
    const std::filesystem::path& projectRoot,
    const std::filesystem::path& relSourceFilePath,
    const std::string& in
) {
    thread_local std::array<char, 2> buff;
    if (
        in.starts_with("//")
        || in.starts_with("http://")
        || in.starts_with("https://")
        // || !in.contains(".md")
    ) {
        return in;
    }

    std::stringstream ss;
    std::stringstream segmentAccumulator;
    ss << in;
    std::vector<std::string> segments;

    if (!in.starts_with("/")) {
        auto preSegments = std::filesystem::relative(
            relSourceFilePath.parent_path(),
            projectRoot
        );

        // prepopulate segments with the path components
        // This is based on the source file's path, so this is safe
        while (preSegments.has_filename()) {
            segments.push_back(preSegments.filename());
            if (!preSegments.has_parent_path()) {
                break;
            }
            preSegments = preSegments.parent_path();
        }
    }

    char ch;

    bool hasPath = false;
    while (ss >> std::noskipws >> ch) {
        if (ch == '/') {
            if (segmentAccumulator.tellp() > 0) {
                segments.push_back(segmentAccumulator.str());
                segmentAccumulator = {};
            }
            hasPath = false;
            continue;
        } else if (ch == '.' && !hasPath) {
            if (ss.peek() == '/') {
                // Dunce /./
                std::ignore = ss.get();
                hasPath = false;
                continue;
            } else if (ss.peek() == '.') {
                size_t curr = ss.tellg();
                std::ignore = ss.get();

                if (ss.peek() == '/') {
                    std::ignore = ss.get();
                    if (segments.size() > 0) {
                        segments.pop_back();
                    }
                    continue;
                }

                ss.seekg(curr);
            }
        } else if (ch == '?' || ch == '#') {
            ss.seekg(size_t(ss.tellg()) - (size_t) 1);
            break;
        }

        hasPath = true;
        segmentAccumulator << ch;
    }
    if (segmentAccumulator.tellp() > 0) {
        segments.push_back(segmentAccumulator.str());
        segmentAccumulator = {};
    }

    std::stringstream rest;
    while (ss && ss >> std::noskipws >> ch) {
        rest << ch;
    }

    if (segments.size() == 0 && rest.tellp() > 0) {
        // No segments but has rest; since no file specified, only return rest (in)
        return in;
    } else if (segments.size() == 0 && rest.tellp() == 0) {
        // Failsafe: force a / when a / is provided
        segments.push_back("");
    }

    std::stringstream parsedPath;
    for (size_t i = 0; i < segments.size(); ++i) {
        auto& segment = segments.at(i);

        parsedPath << "/";
        if (i == segments.size() - 1 && segment.ends_with(".md")) {
            if (segment == "README.md") {
                parsedPath << "index.html";
            } else {
                auto fn = segment.substr(0, segment.rfind('.'));
                std::cout << "fn=" << fn << std::endl;
                parsedPath << renameFile(fn) << ".html";
            }
        } else {
            parsedPath << segment;
        }
    }

    // std::filesystem::path realPath = {
    //     projectRoot.string() + parsedPath.string()
    // };

    std::stringstream escapedUrl;
    urlEncode(
        parsedPath.str(),
        escapedUrl,
        false
    );

    // return pathPrefix + out.str();
    // This is terrible, but the URL shit has infuriated me all night and I just want to get this shit over with
    // The performance this drops is negligible for my use
    auto newPath = pathPrefix;
    if (newPath.ends_with('/') && parsedPath.str().starts_with('/')) {
        newPath += parsedPath.str().substr(1);
    } else {
        newPath += parsedPath.str();
    }

    newPath += rest.str();

    std::cout << "Rewrote " << in << " to " << newPath << std::endl;
    return newPath;
}

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
