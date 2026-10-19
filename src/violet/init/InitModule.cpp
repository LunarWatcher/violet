#include "InitModule.hpp"
#include "minilog/minilog.hpp"
#include "violet/conf/Config.hpp"
#include "violet/data/InitOpts.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

// This pattern is starting to grow on me
namespace {

bool setupConfigFile(std::filesystem::path& resolvedTargetPath, const violet::InitOpts& opts) {
    if (std::filesystem::exists(resolvedTargetPath / "violet.json")) {
        minilog::error(
            "{} already has a violet.json! You currently cannot re-init an initialized folder. If you want to "
            "do so anyway, delete violet.json and rerun the init command.",
            resolvedTargetPath.string()
        );
        return false;
    }
    
    // I'm not a huge fan of setup commands that just require all the info on the command line. As the config file
    // grows, it's much better to defer this to manual editing, which will happen anyway. The template is basically just
    // the quickstart so it doesn't need to be looked up more than anything else.
    violet::Config conf {
        .name = "Placeholder name",
        .description = "Placeholder description",
        .prefix = "/",
        .exclude = {
            "LICENSE",
            "scripts",
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

bool setupPublishScript(const std::filesystem::path& outputPath) {
    // TODO: I'm not super happy with this. If `violet init docs/` in a git repo, the script doesn't end up in the git
    // root, which just feels wrong. It'll still work, but it doesn't feel good.
    // Unfortunately, I believe we're still trying to maintain the possibility for windows support, so
    // stc::Unix::Process won't work for getting the git root. Really need to get around to porting it to Windows as
    // well.
    // Not like it really matters since the publish script requires bash to work, but still
    std::filesystem::path scriptPath = outputPath / "scripts";
    std::filesystem::path publishScript = scriptPath / "publish.sh";

    if (
        !std::filesystem::exists(scriptPath)
        && !std::filesystem::create_directories(scriptPath)
    ) {
        minilog::critical("Failed to create script directory!");
        return false;
    }

    {
        std::ofstream f(publishScript);
        if (!f) {
            minilog::critical("Failed to open {}", publishScript.string());
            return false;
        }

        f << R"(#!/usr/bin/bash

# Setup note: All placeholder values contain FIXME. Search for FIXME and fix all the corresponding things. Make sure to
# remove the error messages as well.

echo "FIXME: Update CB_REMOTE. This must be a valid remote." && exit 1
# Note: this does not need to be a codeberg URL, nor an SSH URL. You can also add more remotes if you want.
CB_REMOTE="git@codeberg.org:FIXME/FIXME"

# Note: We avoid using BASH_SOURCE largely because it seems to be incompatible with worktrees, based on a similar
# problem I ran into at work. There's lots of ways to do this though.
git_root=$(git rev-parse --show-toplevel)
cd "$git_root"

echo "FIXME: Remove this cd if your page is directly under the git root. Otherwise, make sure it's correct." && exit 1
cd FIXME

rm -rf pages

time -- "$violet_exe" generate -d
if [[ $? != 0 ]]; then
    echo "Generation failed"
    exit 1
fi

cd pages
git init
git checkout -b pages
git remote add cb "$CB_REMOTE"
# Example: multiple push remotes
# git remote add gh "git@github.com:user/repo"

git add -A
git commit -m "Generate page"

git push -fu cb pages
# Example(cont.): multiple push remotes
# git push -fu gh pages
)";
    }

    std::filesystem::permissions(
        publishScript,
        // Should be equivalent to chmod +x
        std::filesystem::perms::owner_exec
            | std::filesystem::perms::group_exec
            | std::filesystem::perms::others_exec,
        std::filesystem::perm_options::add
    );
    return true;
}


void dumpHelp() {
    std::cout << R"(
Next steps:

* Edit violet.json to make it project-specific. It currently contains placeholder values.
* Update scripts/publish.sh per the comments in the file. It also contains placeholder values.
* Not sure where to start? There's a guide for getting started you might like:
  https://lunarwatcher.codeberg.page/violet/getting-started.html
* Looking for templating/theming docs? See
  https://lunarwatcher.codeberg.page/violet/templating/index.html

Please note that violet itself is in early development, and may be lacking in documentation.
Well, there's plenty of docs, but they're not necessarily structured well because writing
good docs is fucken hard.

If you struggle with the docs, please open an issue:
  https://codeberg.org/LunarWatcher/violet
)";
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

    if (!setupPublishScript(targetFolder)) {
        minilog::error("Failed to initialize publish script!");
        return 1;
    }

    minilog::info("Violet project successfully initialized");

    dumpHelp();
    return 0;
}
