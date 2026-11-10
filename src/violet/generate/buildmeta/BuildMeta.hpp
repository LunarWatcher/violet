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
    Config& conf;
    meta::MetadataJson data;
public:
    BuildMeta(
        Config& conf
    );

    void registerInputFile();
    void registerInputPage();
    void registerOutputPage(uint64_t batchSize = 1);
    void start() {
        data = {};
    }

    void commit();
};

}
