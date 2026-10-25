#include "Frontmatter.hpp"
#include "iris/data/DataObject.hpp"
#include "violet/data/Constants.hpp"
#include "violet/generate/ProcessedFileType.hpp"

namespace violet {

Frontmatter::operator iris::DataObject() const {
    auto obj = iris::Object {
        {"title", *title}, // NOLINT(bugprone-unchecked-optional-access): this is guaranteed populated by the time this
                           // is called
        {"type", type},
        {"layout", getLayout()},
        {"hidden", hidden},
        {"data", data},
        {"url", internalUrl},
        {"table_of_contents", tableOfContents},
        {violet::constants::InternalPathRef, internalPath},
        {violet::constants::InternalFileTypeRef, static_cast<int>(internalFileType)},
    };
    if (date) {
        obj["date"] = *date;
    }
    if (last_modified) {
        obj["last_modified"] = *last_modified;
    }
    if (summary) {
        obj["summary"] = *summary;
    }

    auto taxonomies = iris::Object {};

    for (const auto& [k, v] : this->taxonomies) {
        taxonomies[k] = std::vector<std::string>(
            v.begin(),
            v.end()
        );
    }
    obj["taxonomies"] = std::move(taxonomies);

    return obj;
}

Frontmatter Frontmatter::fromDataObject(const iris::Object& src) {
    Frontmatter f;

    f.title = src.at("title").get<iris::String>();
    f.type = src.at("type").get<iris::String>();
    f.layout = src.at("layout").get<iris::String>();
    f.hidden = src.at("hidden").get<bool>();
    f.data = src.at("data").get<iris::Object>();
    f.internalUrl = src.at("url").get<iris::String>();
    f.tableOfContents = src.at("table_of_contents").get<iris::String>();
    f.internalPath = src.at(violet::constants::InternalPathRef).get<iris::String>();
    f.internalFileType = static_cast<ProcessedFileType>(src.at(violet::constants::InternalFileTypeRef).get<iris::Int>());

    if (src.contains("date")) {
        f.date = src.at("date").get<iris::Int>();
    }
    if (src.contains("last_modified")) {
        f.last_modified = src.at("last_modified").get<iris::Int>();
    }
    if (src.contains("summary")) {
        f.summary = src.at("summary").get<iris::String>();
    }

    auto taxonomies = src.at("taxonomies").get<iris::Object>();
    for (const auto& [k, v] : taxonomies) {
        std::unordered_set<std::string> it;
        for (auto& l : v.get<iris::Array>()) { it.insert(l.get<iris::String>()); }
        f.taxonomies[k] = it;
    }

    return f;
}

}
