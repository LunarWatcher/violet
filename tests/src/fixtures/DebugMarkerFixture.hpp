#pragma once

#include "catch2/catch_test_macros.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>

namespace tests {

struct DebugMarkerFixture {
    static inline std::regex extract {
        R"(\[\[violet-test\(([^)]+)\):(\|*)(.*?)\2\]\])",
        std::regex::optimize | std::regex::ECMAScript
    };

    std::string createDebugMarker(const std::string& key, const std::string& content) const {
        return std::format(
            R"(\[\[violet-test({}):||||{}||||\]\])" "\n",
            key,
            content
        );
    }

    std::map<std::string, std::string> getDebug(
        const std::string& filename = "debug.html"
    ) const {
        auto p = output() / filename;
        INFO("Expected output file: " << p.string());
        std::ifstream f(p);
        REQUIRE(bool(f));
        std::stringstream ss;
        ss << f.rdbuf();
        std::string buff = ss.str();

        std::map<std::string, std::string> out;
        for (auto it = std::sregex_iterator(buff.begin(), buff.end(), extract); it != std::sregex_iterator(); ++it) {
            const std::smatch& match = *it;

            out[match.str(1)] = match.str(3);
        }
        return out;
    }

    /**
     * Returns the output folder.
     */
    virtual std::filesystem::path output() const = 0;
};

}
