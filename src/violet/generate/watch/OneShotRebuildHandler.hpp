#pragma once

#include "violet/data/GenerateOpts.hpp"
#include "AutoRebuildHandler.hpp"

namespace violet {

class OneShotRebuildHandler : public AutoRebuildHandler {
public:
    OneShotRebuildHandler(
        const GenerateOpts& opts
    ): AutoRebuildHandler(opts) {}
    virtual ~OneShotRebuildHandler() = default;

    int watch() override;
};

}
