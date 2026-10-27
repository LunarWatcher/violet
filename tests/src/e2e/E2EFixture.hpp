#pragma once

#include "catch2/catch_test_macros.hpp"
#include "parsing/markdown/fixture/MarkdownFixture.hpp"
#include "stc/test/TestDirectory.hpp"
#include "violet/CommandLine.hpp"
#include "violet/conf/Config.hpp"
#include <filesystem>
#include <fstream>
#include <regex>

namespace tests {

struct E2EFixture : public MarkdownFixture {
    const std::filesystem::path dir = std::filesystem::temp_directory_path() / "violet-tests";
    stc::testutil::TestDirectory dirLock{ dir, true };

    violet::Config conf {
        .name = "E2E test site",
        .description = "Description content",
        .prefix = "/prefix",
        .exclude = {
            "LICENSE"
        },
        .data = iris::Object(),
        .metadata = true,
    };

    static inline std::regex extract {
        R"(\[\[violet-test\(([^)]+)\):(.*?)\]\])",
        std::regex::optimize
    };

    std::string expectedOutputFolder = "pages";

    E2EFixture() noexcept = default;

    E2EFixture& loadSite() {
        nlohmann::json j = conf;
        withFile("violet.json", j.dump(2));
        withFile(
            "README.md",
            R"(# Hello world!
This is a placeholder site for violet used in E2E tests.
)"
        );
        withFile("debug.md", createDebugMarker("prefix", "{{ site.prefix }}"));
        return *this;
    }

    std::string createDebugMarker(const std::string& key, const std::string& content) {
        return std::format(
            R"(\[\[violet-test({}):{}\]\])",
            key,
            content
        );
    }

    E2EFixture& withFile(const std::filesystem::path& relPath, const std::string& content) {
        auto p = dir / relPath;
        INFO("Creating " << p.string());
        std::filesystem::create_directories(p.parent_path());
        std::ofstream f(p);
        REQUIRE(bool(f));

        f << content;
        return *this;
    }

    E2EFixture& withOutputFolder(const std::string& folder) {
        this->expectedOutputFolder = folder;
        return *this;
    }

    void run(
        const std::vector<std::string>& argv,
        bool expectError = false
    ) {
        std::string rootFlag = std::format(
            "--root"
        );
        std::string rootArg = dir.string();
        std::vector<char*> argvCast;
        argvCast.reserve(argv.size() + 3);
        argvCast.push_back((char*) "violet");
        bool hasProcessedCommand = false;
        for (auto& str : argv) {
            argvCast.push_back((char*) str.data());
            if (!hasProcessedCommand && str == "generate") {
                argvCast.push_back((char*) rootFlag.data());
                argvCast.push_back((char*) rootArg.data());
            }
            hasProcessedCommand = true;
        }
        auto shim = [&]() {
            auto val = violet::cliMain(
                (int) argvCast.size(),
                (char**) argvCast.data()
            );

            if (val != 0) {
                throw std::runtime_error(
                    "cliMain returned non-0"
                );
            }
        };
        if (expectError) {
            REQUIRE_THROWS(shim());
        } else {
            shim();
        }
    }

    void autoBuild(bool expectError = false) {
        run({
            "generate", "-d"
        }, expectError);

        if (!expectError) {
            requireOutput();
        }

    }

    void requireOutput() {
        auto path = output();
        INFO("Expected output folder: " << path.string());
        REQUIRE(
            std::filesystem::exists(path)
        );
    }

    void requireOutputFileExists(
        const std::string& relPath
    ) {
        auto p = output() / relPath;
        INFO("Resolved output path: " << p.string());
        REQUIRE(
            std::filesystem::exists(p)
        );
    }

    std::map<std::string, std::string> getDebug(
        const std::string& filename = "debug.html"
    ) {
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

            out[match.str(1)] = match.str(2);
        }
        return out;
    }

    const std::filesystem::path& root() {
        return dir;
    }
    /**
     * Get the output folder. The name param is only required if you're using a non-standard output folder, as that's a
     * command line flag that cannot be accessed trivially through code.
     */
    std::filesystem::path output() {
        return dir / expectedOutputFolder;
    }
};

}
