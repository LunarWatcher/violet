#pragma once

#include "violet/data/GenerateOpts.hpp"
#include "violet/generate/SiteGenerator.hpp"
namespace violet {

class AutoRebuildHandler {
protected:
    GenerateOpts opts;

    std::shared_ptr<SiteGenerator> generator;

    bool refreshGenerator() {
        this->generator = nullptr;
        try {
            auto siteGenResult = SiteGenerator::loadWorkspace(opts);
            if (siteGenResult.has_value()) {
                this->generator = siteGenResult.value();
            } else {
                std::cerr << "Failed to load workspace: " << siteGenResult.error() << std::endl;
                return false;
            }
        } catch (const nlohmann::json::exception& e) {
            minilog::error("Failed to deserialize violet.json: {}", e.what());
            return false;
        }
        return true;
    }

    bool generate() {
        auto genRes = generator->generate();
        if (!genRes) {
            std::cerr << "Site generation failed. See error log" << std::endl;
            return false;
        }
        return true;
    }
public:
    AutoRebuildHandler(const GenerateOpts& opts)
        : opts(opts) {}
    virtual ~AutoRebuildHandler() = default;

    virtual int watch() = 0;
};

}
