#include "CommandLine.hpp"
#include "CLI/CLI.hpp"
#include "minilog/minilog.hpp"
#include "violet/generate/SiteGenerator.hpp"
#include <CLI/CLI.hpp>

void violet::withStandardFlags(CLI::App* app) {
    app->add_flag_function(
        "-d,--debug",
        [](bool debug) {
            // This should never come up, but it's at least nice to provide the option for scripts to use
            // `--debug=${computed-debug-var}` based on outer scripting logic
            if (debug) {
                minilog::setLevel(minilog::Level::Debug);
            }
        },
        "Whether or not to enable verbose debug logging"
    );
}

int violet::cliMain(int argc, char** argv) {
    CLI::App app {
        "C++-based static site generator",
    };
    GenerateOpts generateOpts;

    minilog::setLevel(minilog::Level::Info);
    app.require_subcommand(1, 1);

    argv = app.ensure_utf8(argv);

    auto cmdVersion = app.add_subcommand(
        "version",
        "Outputs the version"
    );

    auto cmdGenerate = app.add_subcommand(
        "generate",
        "Converts the input project (always in the working directory) to a generated site"
    );
    withStandardFlags(cmdGenerate);
    cmdGenerate->add_option(
        "-w,--watch",
        generateOpts.watch,
        "Whether or not to watch for changes. Not setting this will generate the site and then immediately exit."
    );
    cmdGenerate->add_flag(
        "-l,--local",
        generateOpts.overridePrefixForLocalUse,
        "Whether or not to enable local extensions in the build. This makes a build that's "
        "incompatible with deployment outside your local machine."
    )
        ->default_val(generateOpts.overridePrefixForLocalUse);

    auto cmdServe = app.add_subcommand(
        "serve",
        "Currently not implemented, but will exist in the future:tm:"
    );
    withStandardFlags(cmdServe);

    CLI11_PARSE(app, argc, argv);

    if (cmdVersion->parsed()) {
        std::cout << "violet " << VIOLET_VERSION << std::endl;
    } else if (cmdGenerate->parsed()) {
        return generateMain(generateOpts);
    } else if (cmdServe->parsed()) {
        std::cout
            << "The --help said the serve command wasn't implemented yet. it said this because this command has not "
            "been implemented. What did you expect this would do?"
            << std::endl;
        return -1;
    }
    return 0;
}

int violet::generateMain(const GenerateOpts& opts) {
    auto siteGenResult = SiteGenerator::loadWorkspace(opts);
    if (siteGenResult.has_value()) {
        auto& ptr = siteGenResult.value();
        auto genRes = ptr->generate();
        if (!genRes) {
            std::cerr << "Site generation failed. See error log" << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Failed to load workspace: " << siteGenResult.error() << std::endl;
        return 1;
    }
    return 0;
}
