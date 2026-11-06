#pragma once

#include "AutoRebuildHandler.hpp"

#ifdef ENABLE_WATCH_HANDLER
#undef ENABLE_WATCH_HANDLER
#endif

#ifdef __linux__
#define ENABLE_WATCH_HANDLER

#include <stc/filewatcher/FileWatcher.hpp>
#include <atomic>
#else
#include "OneShotRebuildHandler.hpp"
#endif

namespace violet {

#ifdef ENABLE_WATCH_HANDLER
class WatchRebuildHandler : public AutoRebuildHandler {
private:
    stc::FileWatcher watcher;
    std::atomic<bool>* notify;
public:
    WatchRebuildHandler(
        const GenerateOpts &opts,
        std::atomic<bool>* notify = nullptr
    );
    virtual ~WatchRebuildHandler() = default;

    int watch() override;
};
#else
using WatchRebuildHandler = OneShotRebuildHandler;
#endif

}
