#include "InitModule.hpp"
#include "minilog/minilog.hpp"
#include "violet/conf/Config.hpp"
#include "violet/data/InitOpts.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

// This pattern is starting to grow on me
namespace {

bool setupConfigFile(std::filesystem::path& resolvedTargetPath, const violet::InitOpts& opts) {
    if (std::filesystem::exists(resolvedTargetPath / "violet.json")) {
        minilog::error("{} already has a violet.json!", resolvedTargetPath.string());
        return false;
    }
    
    // I'm not a huge fan of setup commands that just require all the info on the command line. As the config file
    // grows, it's much better to defer this to manual editing, which will happen anyway. The template is basically just
    // the quickstart so it doesn't need to be looked up more than anything else.
    violet::Config conf {
        .name = "Placeholder name",
        .description = "Placeholder description",
        .exclude = {
            "LICENSE",
        },
        .theme = "_default",
        .data = nlohmann::json::object(),
        .metadata = false,
    };

    try {
        conf.setPrefix(opts.prefix);
    } catch (const std::runtime_error&) {
        // TODO: it would be better if setPrefix returned a bool rather than throw, but this will complicate error
        // handling in the generation parts of the code that have a much more indirect data flow between error handling
        // locations. Really should look into how to better design code to allow that from the start.
        // (It's mostly just a nice to have, exception overhead is so minimal it doesn't matter when it's a termination
        // point anyway)
        return false;
    }

    std::ofstream confFile(resolvedTargetPath / "violet.json");

    if (!confFile) {
        minilog::critical("Failed to open {}!", resolvedTargetPath.string());
        return false;
    }

    nlohmann::json j = conf;
    confFile << j.dump(4) << "\n";

    return true;
}

}

int violet::initMain(const violet::InitOpts& opts) {
    std::filesystem::path targetFolder = std::filesystem::weakly_canonical(
        std::filesystem::current_path() / opts.folder
    );

    if (!std::filesystem::exists(targetFolder)) {
        std::filesystem::create_directories(targetFolder);
    }

    // standalone if just in case create_directories does something weird. I think it throws if creation fails, but
    // can't be bothered checking
    if (!std::filesystem::is_directory(targetFolder)) {
        minilog::error("{} is not a folder!", targetFolder.string());
        return 1;
    }

    if (!setupConfigFile(targetFolder, opts)) {
        minilog::error("Config file init failed!");
        return 1;
    }

    minilog::info("Violet project successfully initialized");
    return 0;
}
