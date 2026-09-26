#pragma once

#include "catch2/catch_test_macros.hpp"
#include "fixtures/LoadWorkspace.hpp"
#include "nlohmann/json.hpp"
#include "stc/test/TestDirectory.hpp"
#include "violet/CommandLine.hpp"
#include "violet/conf/Config.hpp"
#include <filesystem>
#include <fstream>
#include <thread>

namespace tests {

struct E2EFixture {
    const std::filesystem::path dir = std::filesystem::temp_directory_path() / "violet-tests";
    stc::testutil::TestDirectory dirLock{ dir, true };

    violet::Config conf {
        .name = "E2E test site",
        .description = "Description content",
        .prefix = "/prefix",
        .exclude = {
            "LICENSE"
        },
        .data = nlohmann::json::object(),
        .metadata = true,
    };

    E2EFixture() noexcept {
        conf.imbueRoot(dir);
    }


    void loadSite() {
        {
            nlohmann::json j = conf;
            std::ofstream f(dir / "violet.json");
            INFO(dir / "violet.json");
            REQUIRE(bool(f));
            f << j;
        }

        {
            std::ofstream f(dir / "README.md");
            REQUIRE(bool(f));
            f << R"(# Hello world!
This is a placeholder site for violet used in E2E tests.
)";
        }

        {
            std::ofstream f(dir / "debug.md");
            REQUIRE(bool(f));
            f << R"(
\[\[violet-test(prefix):{{site.prefix}}\]\]
)";
        }
        // TODO: how do we want to add more test data here? Test data builder maybe? It feels like basing the tests on
        // the hard-coded paths makes more sense for the integration tests, but they still lock down quite a bit.
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

    void requireOutput(
        const std::string& outputFolderName = "pages"
    ) {
        auto path = output(outputFolderName);
        INFO("Expected output folder: " << path.string());
        REQUIRE(
            std::filesystem::exists(path)
        );
    }

    std::map<std::string, std::string> getDebug(
        const std::string& outputFolderName = "pages"
    ) {
        auto p = output(outputFolderName);
        INFO("Expected output folder: " << p.string());
        std::ifstream f(p / "debug.html");
        REQUIRE(bool(f));
        std::stringstream ss;
        ss << f.rdbuf();
        std::string buff = ss.str();
        std::regex extract(
            R"(\[\[violet-test\(([^)]+)\):(.*?)\]\])"
        );

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
    std::filesystem::path output(
        const std::string& outputFolderName = "pages"
    ) {
        return dir / outputFolderName;
    }
};

}
