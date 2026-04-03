#pragma once

namespace violet {

struct GenerateOpts {
    /**
     * Whether or not to watch for changes.
     */
    bool watch = false;
};

extern int generateMain(const GenerateOpts& opts);

extern int cliMain(
    int argc, char** argv
);

}
