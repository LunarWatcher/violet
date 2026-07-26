#include "BuildMetaJson.hpp"

namespace violet {

void meta::to_json(nlohmann::json& out, const Input& in) {
    out["pages"] = in.pages;
    out["files"] = in.files;
}

void meta::to_json(nlohmann::json& out, const Output& in) {
    out["pages"] = in.pages;
}

void meta::to_json(nlohmann::json& out, const Build& in) {
    out["time_ms"] = in.buildMillis;
    out["started_at"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        in.startTimestamp.time_since_epoch()
    ).count();
    out["version"] = in.version;
}

void meta::to_json(nlohmann::json& out, const MetadataJson& in) {
    out["input"] = in.input;
    out["output"] = in.output;
    out["build"] = in.build;
}

void meta::Build::commitBuildMillis() {
    auto end = std::chrono::system_clock::now();
    auto buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - startTimestamp
    ).count();
    buildMillis = buildTime;
}

}
