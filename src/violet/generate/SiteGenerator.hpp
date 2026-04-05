#pragma once

#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"
#include <expected>
#include <memory>
#include <string_view>

namespace violet {

class SiteGenerator {
private:
    GenerateOpts opts;
    Config cfg;

    SiteGenerator(
        const GenerateOpts& opts,
        Config&& workspaceConf
    );
public:
    ~SiteGenerator() = default;

    /**
     * Generates the site
     *
     * \returns whether or not the generation was successfully completed.
     */
    bool generate();

    /**
     * Returns a SiteGenerator initialised with the provided GenerateOpts, provided a `violet.json` is found in the
     * current working directory.
     * Otherwise, or in the event of any number of other init errors related to config, a string representing a type of
     * error is returned. This string should not be parsed programmatically, as it's intended for non-recoverable issues
     * that require user intervention.
     */
    static std::expected<std::shared_ptr<SiteGenerator>, std::string_view> loadWorkspace(
        const GenerateOpts& opts
    );
};

}
