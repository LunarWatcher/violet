#pragma once

#include "violet/data/GenerateOpts.hpp"

namespace violet {

extern int generateMain(const GenerateOpts& opts);

extern int cliMain(
    int argc, char** argv
);

}
