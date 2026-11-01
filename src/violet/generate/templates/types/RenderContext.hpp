#pragma once

#include "violet/conf/Frontmatter.hpp"
#include <iris/iris.hpp>

namespace violet {

struct ListingRenderContext {
    int64_t page;
    int64_t total_pages;
    std::vector<Frontmatter> pages;
    std::filesystem::path base_path;

    operator iris::DataObject() const;
};

/**
 * This represents the object that is passed as context to each render. This struct is also reused as userdata.
 *
 * The `violet` global, though not present in this struct, is also added when converting to a DataObject.
 */
struct RenderContext {
    const Frontmatter& fm;
    // This is just used raw since we don't need/want to reconstruct it. The functions using this struct as udata don't
    // need to care since the context is provided in each class anyway, and it can be treated as a global
    const iris::Object& site;

    std::optional<ListingRenderContext> listing = std::nullopt;

    operator iris::DataObject() const;
};

}
