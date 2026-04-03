#include "CommandLine.hpp"
#include "CLI/CLI.hpp"
#include <CLI/CLI.hpp>

int violet::cliMain(int argc, char** argv) {
    CLI::App app{
        "C++-based static site generator"
    };
    app.require_subcommand(1, 1);

    argv = app.ensure_utf8(argv);

    GenerateOpts generateOpts;

    auto cmdGenerate = app.add_subcommand(
        "generate",
        "Converts the input project (always in the working directory) to a generated site"
    );
    cmdGenerate->add_option(
        "-w,--watch",
        generateOpts.watch,
        "Whether or not to watch for changes. Not setting this will generate the site and then immediately exit."
    );

    auto cmdServe = app.add_subcommand(
        "serve",
        "Currently not implemented, but will exist in the future:tm:"
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
    std::cout << "TODO" << std::endl;
    return 0;
}
