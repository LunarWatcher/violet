#include "OneShotRebuildHandler.hpp"

namespace violet {

int OneShotRebuildHandler::watch() {
    if (!this->refreshGenerator()) {
        return 1;
    }
    auto res = this->generate();
    // This is a fallback so we don't need a whole extra type that's going to be identical to this handler, but also
    // provide an error message
    if (opts.watch) {
        minilog::error(
            "You supplied --watch, but unfortunately, your platform is not compatible with this feature. "
            "Violet will now exit."
        );
        return 1;
    }
    return res ? 0 : 1;
}

}
