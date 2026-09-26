#pragma once

#include "violet/data/GenerateOpts.hpp"
#include "violet/data/InitOpts.hpp"
#include <CLI/CLI.hpp>

namespace violet {

extern CLI::App* generateCommand(CLI::App& app, violet::GenerateOpts& generateOpts);
extern CLI::App* initCommand(CLI::App& app, violet::InitOpts& initOpts);
extern CLI::App* serveCommand(CLI::App& app);
extern CLI::App* versionCommand(CLI::App& app);

extern int generateMain(const GenerateOpts& opts);
// This may scale up to a full builder at some point, but for now, I only need --debug everywhere
extern void withStandardFlags(CLI::App* app);

extern int cliMain(
    int argc, char** argv
);

}
