#include "BuildMeta.hpp"
#include "minilog/minilog.hpp"

#include <chrono>
#include <fstream>

namespace violet::meta {

BuildMeta::BuildMeta(
    GenerateOpts& opts,
    Config& conf
)
    : opts(opts),
      conf(conf) {}

void BuildMeta::registerInputFile() {
    data.input.files += 1;
}
void BuildMeta::registerInputPage() {
    data.input.pages += 1;
}
void BuildMeta::registerOutputPage(uint64_t batchSize) {
    data.output.pages += batchSize;
}

void BuildMeta::commit() {
    this->data.build.commitBuildMillis();

    // We only write the file if metadata is enabled. If it isn't, all we're doing is incrementing some ints, which in
    // terms of performance might be better than null/empty checks everywhere. Not actually sure about that, but /shrug
    if (this->conf.metadata) {
        nlohmann::json j = data;
        auto metaPath = std::filesystem::path(opts.outputFolder) / "_violet-meta.json";
        std::ofstream f(metaPath);

        if (!f) {
            minilog::error("Failed to open {}", metaPath.string());
            throw std::runtime_error("Failed to generate metadata");
        }

        f << j;
    }
}

}
