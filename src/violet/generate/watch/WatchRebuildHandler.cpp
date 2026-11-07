#include "WatchRebuildHandler.hpp"
#include "stc/filewatcher/FileWatcherCommon.hpp"

#ifdef ENABLE_WATCH_HANDLER

#include <chrono>
#include <filesystem>

namespace violet {

WatchRebuildHandler::WatchRebuildHandler(
    const GenerateOpts &opts,
    std::atomic<bool>* notify
) : AutoRebuildHandler(opts),
    watcher(
        { opts.root },
        {
            .minimumDelay = std::chrono::milliseconds(1000)
        },
        [this](const auto &path) {
            auto &opts = this->opts;
            return !std::filesystem::weakly_canonical(path).string()
                .starts_with(
                    std::filesystem::weakly_canonical(opts.root / opts.outputFolder).string()
                );
        }
    ), notify(notify) {}

int WatchRebuildHandler::watch() {
    if (!refreshGenerator()) {
        minilog::error(
            "Initial site generation failed. Please correct violet.json and try again"
        );
    } else {
        auto result = generate();
        this->notify->store(result);
    }
    watcher.block([this](auto type, const auto &path) -> bool {
        if (
            (type == stc::watcher::FileWatcherEventKind::DirectoryAdded
                || type == stc::watcher::FileWatcherEventKind::DirectoryRemoved
            )
            && std::filesystem::weakly_canonical(path).string()
                .starts_with(
                    std::filesystem::weakly_canonical(opts.root / opts.outputFolder).string()
                )
        ) {
            // Avoid weird shit when running the update script
            minilog::info(
                "Note: detected a deletion and/or creation event for the generated output folder. Ignoring this "
                "update event."
            );
            return true;
        }
        
        minilog::info("Changes detected in {}", path.string());
        if (path == opts.root / "violet.json") {
            if (!this->refreshGenerator()) {
                minilog::error("Invalid violet.json.");
                return true;
            }
        } else if (this->generator == nullptr) {
            minilog::error("Cannot generate page until violet.json is fixed. "
                "Attempting refresh even though it wasn't "
                "detected as edited");
            if (!this->refreshGenerator()) {
                minilog::error("violet.json is still invalid");
                return true;
            } else {
                minilog::info("violet.json has been fixed. Rebuilding...");
            }
        }
        // TODO: it would be nice if we could serve the error message directly on the website, but this is currently not
        // possible due to how the error handling is managed. Maybe if like `lastError` is stored as a string or
        // something?
        if (generate()) {
            minilog::info("Site successfully refreshed");
            if (this->notify) {
                this->notify->store(true);
            }
        } else {
            minilog::error("Site failed to generate. See error messages");
            if (this->notify) {
                this->notify->store(false);
            }
        }

        return true;
    });
    return 0;
}
} // namespace violet

#endif
