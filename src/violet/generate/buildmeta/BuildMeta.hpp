#pragma once

#include "BuildMetaJson.hpp"
#include "violet/conf/Config.hpp"
#include "violet/data/GenerateOpts.hpp"

namespace violet::meta {

/**
 * Helper class for generating metadata
 */
class BuildMeta {
private:
    GenerateOpts& opts;
    Config& conf;
    meta::MetadataJson data;
public:
    BuildMeta(
        GenerateOpts& opts,
        Config& conf
    );

    void registerInputFile();
    void registerInputPage();
    void registerOutputPage(uint64_t batchSize = 1);

    void commit();
};

}
