#pragma once

#include "violet/data/GenerateOpts.hpp"
#include <CLI/CLI.hpp>

namespace violet {

extern int generateMain(const GenerateOpts& opts);
// This may scale up to a full builder at some point, but for now, I only need --debug everywhere
extern void withStandardFlags(CLI::App* app);

extern int cliMain(
    int argc, char** argv
);

}
