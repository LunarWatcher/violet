#pragma once
// Must always be first
#include "violet/generate/SiteGenerator.hpp"
#include "violet/generate/watch/WatchRebuildHandler.hpp"
#include "violet/serve/meta/include-guard.hpp"

#include "violet/data/GenerateOpts.hpp"

#include "magpie/data/CommonData.hpp"

namespace violet {

struct ServerContext : public magpie::data::CommonData {
    GenerateOpts generateOpts;
    std::shared_ptr<WatchRebuildHandler> watcher;

    ServerContext(
        const GenerateOpts& generateOpts,
        const std::shared_ptr<WatchRebuildHandler>& watcher
    )
        : generateOpts(generateOpts), watcher(watcher)
    {}
};

}
