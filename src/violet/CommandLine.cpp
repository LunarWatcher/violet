#include "CommandLine.hpp"
#include "CLI/CLI.hpp"
#include "minilog/minilog.hpp"
#include "violet/generate/SiteGenerator.hpp"
#include <CLI/CLI.hpp>

int violet::cliMain(int argc, char** argv) {
    CLI::App app{
        "C++-based static site generator"
    };
    GenerateOpts generateOpts;

    minilog::setLevel(minilog::Level::Info);

    app.require_subcommand(1, 1);

    argv = app.ensure_utf8(argv);


    auto cmdGenerate = app.add_subcommand(
        "generate",
        "Converts the input project (always in the working directory) to a generated site"
    );
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

    cmdGenerate->add_flag_function(
        "-d,--debug",
        [](const auto&) {
            minilog::setLevel(minilog::Level::Debug);
        },
        "Whether or not to enable verbose debug logging"
    );
    auto cmdServe = app.add_subcommand(
        "serve",
        "Currently not implemented, but will exist in the future:tm:"
    );
    cmdServe->add_flag_function(
        "-d,--debug",
        [](const auto&) {
            minilog::setLevel(minilog::Level::Debug);
        },
        "Whether or not to enable verbose debug logging"
    );

    CLI11_PARSE(app, argc, argv);

    if (cmdGenerate->parsed()) {
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
