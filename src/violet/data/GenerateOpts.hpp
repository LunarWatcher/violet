#pragma once

#include <string>

namespace violet {

struct GenerateOpts {
    /**
     * Whether or not to watch for changes.
     */
    bool watch = false;

    /**
     * The path to output into
     *
     * If the the folders or intermediate folders do not exist, they will be created on the fly.
     */
    std::string outputFolder = "pages";
};

}
