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

void meta::from_json(const nlohmann::json& in, Input& out) {
    in["pages"].get_to(out.pages);
    in["files"].get_to(out.files);
}
void meta::from_json(const nlohmann::json& in, Output& out) {
    in["pages"].get_to(out.pages);
}
void meta::from_json(const nlohmann::json& in, Build& out) {
    in["time_ms"].get_to(out.buildMillis);
}
void meta::from_json(const nlohmann::json& in, MetadataJson& out) {
    in["input"].get_to(out.input);
    in["output"].get_to(out.output);
    in["build"].get_to(out.build);
}

void meta::Build::commitBuildMillis() {
    auto end = std::chrono::system_clock::now();
    auto buildTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - startTimestamp
    ).count();
    buildMillis = buildTime;
}

}
