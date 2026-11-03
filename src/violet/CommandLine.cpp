#include "CommandLine.hpp"
#include "CLI/CLI.hpp"
#include "minilog/minilog.hpp"
#include "violet/data/GenerateOpts.hpp"
#include "violet/data/GitVersion.hpp"
#include "violet/generate/SiteGenerator.hpp"
#include "violet/init/InitModule.hpp"
#include <CLI/CLI.hpp>
#include <filesystem>
#ifdef VIOLET_ENABLE_SERVER
#include "violet/serve/SiteServer.hpp"
#endif

namespace {

void commonGenerateFlags(
    CLI::App* cmd,
    violet::GenerateOpts& generateOpts
) {
    
    cmd->add_option(
        "-r,--root",
        generateOpts.root,
        "Override the root. This defaults to the current working directory, and must be an absolute path"
    )
        ->default_val(std::filesystem::current_path());
}

}

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

CLI::App* violet::generateCommand(CLI::App& app, violet::GenerateOpts& generateOpts) {
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

    auto* local = cmdGenerate->add_flag(
        "-l,--local",
        generateOpts.overridePrefixForLocalUse,
        "Whether or not to enable local extensions in the build. This makes a build that's "
        "incompatible with deployment outside your local machine."
    )
        ->default_val(generateOpts.overridePrefixForLocalUse);
    cmdGenerate->add_option(
        "-p,--prefix",
        generateOpts.overridePrefix,
        "Override the prefix to a custom string. Avoid using this if you can, and prefer to decide "
        "whether or not you actually want to change the prefix set by your `violet.json`, or if you want --local "
        "instead. This is useful if you want to use Firefox' local mode mappings"
    )
        ->excludes(local);

    commonGenerateFlags(
        cmdGenerate, generateOpts
    );

    cmdGenerate->add_flag(
        "--legacy-extensions",
        generateOpts.allowLegacyInjaExtensions,
        "Lets projects that still use the .inja extensions work (in theory). Using this flag is discouraged; "
        "please switch `.inja` to `.iris`. This flag and the support for `.inja` extensions will be removed in v1."
    )
        ->default_val(false);

    return cmdGenerate;
}

CLI::App* violet::initCommand(CLI::App& app, violet::InitOpts& initOpts) {
    auto cmdInit = app.add_subcommand(
        "init",
        "Initializes a specified folder with a violet.json."
    );

    cmdInit->add_option(
        "folder",
        initOpts.folder,
        "What folder to generate in"
    )
        ->required(true);


    return cmdInit;
}

CLI::App* violet::serveCommand(CLI::App& app, violet::GenerateOpts& generateOpts) {
    auto cmdServe = app.add_subcommand(
        "serve",
        "Currently not implemented, but will exist in the future:tm:"
    );
    withStandardFlags(cmdServe);
    commonGenerateFlags(cmdServe, generateOpts);

    return cmdServe;
}

CLI::App* violet::versionCommand(CLI::App& app) {
    auto cmdVersion = app.add_subcommand(
        "version",
        "Outputs the version"
    );

    return cmdVersion;
}

void violet::initMeta(CLI::App& app) {
    app.footer(R"(
Violet is open-source software licensed under the MIT license.
For the license text, see https://codeberg.org/LunarWatcher/violet/src/branch/master/LICENSE
The source code can be found on codeberg: https://codeberg.org/LunarWatcher/violet

Violet uses the following libraries:

 * LunarWatcher/minilog (MIT license): https://codeberg.org/LunarWatcher/minilog/src/branch/master/LICENSE
 * LunarWatcher/stc (MIT license): https://codeberg.org/LunarWatcher/stc
 * LunarWatcher/cli12 (3-clause BSD): https://codeberg.org/LunarWatcher/CLI12/src/branch/main/LICENSE
 * LunarWatcher/iris (MIT license): https://codeberg.org/LunarWatcher/iris/src/branch/master/LICENSE
 * nlohmann/json (MIT license; version pinned to v3.12 in fork): https://github.com/LunarWatcher/json/blob/master/LICENSE.MIT

If violet was compiled with serving enabled, the following additional libraries are used:
 * LunarWatcher/magpie (MIT license): https://codeberg.org/LunarWatcher/magpie/src/branch/master/LICENSE
 * LunarWatcher/raven (MIT license): https://codeberg.org/LunarWatcher/raven/src/branch/master/LICENSE
 * OpenSSL (Apache 2.0): https://github.com/openssl/openssl/blob/master/LICENSE.txt
 * zlib (zlib license): https://github.com/madler/zlib/blob/develop/LICENSE

Additionally, the following libraries are used in tests, and are therefore not distributed alongside the main binary:

 * catchorg/catch2 (Boost Software License 1.0): https://github.com/catchorg/Catch2/blob/devel/LICENSE.txt
)");

}

int violet::cliMain(int argc, char** argv) {
    CLI::App app {
        "C++-based static site generator",
    };
    GenerateOpts generateOpts, generateServeOpts {
        .overridePrefix = "/"
    };
    InitOpts initOpts;

    minilog::setLevel(minilog::Level::Info);
    app.require_subcommand(1, 1);

    argv = app.ensure_utf8(argv);

    auto* cmdGenerate = generateCommand(app, generateOpts);
    auto* cmdInit = initCommand(app, initOpts);
    auto* cmdServe = serveCommand(app, generateServeOpts);
    auto* cmdVersion = versionCommand(app);

    initMeta(app);

    CLI12_PARSE(app, argc, argv);

    if (cmdVersion->parsed()) {
        std::cout << "violet " << violet::GitVersion() << std::endl;
    } else if (cmdGenerate->parsed()) {
        return generateMain(
            generateOpts
        );
    } else if (cmdServe->parsed()) {
        return serveMain(
            generateServeOpts
        );
    } else if (cmdInit->parsed()) {
        return initMain(initOpts);
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

#ifdef VIOLET_ENABLE_SERVER
int violet::serveMain(
    const GenerateOpts& generateOpts
) {
    SiteServer serv(
        generateOpts
    );
    serv.serve();
    return 0; // serve does not return
}
#else
int violet::serveMain(
    const GenerateOpts&
) {
    std::cerr
        << "Violet was compiled without server support. You can still preview your site in your browser. "
        "run `violet generate -l`, or run `violet generate -p /` and use firefox local mode: "
        "https://firefox-source-docs.mozilla.org/devtools-user/local_mode/index.html . For more info, see "
        "https://lunarwatcher.codeberg.page/violet/previewing-a-site.html"
        << std::endl;
    return 1;
}
#endif
