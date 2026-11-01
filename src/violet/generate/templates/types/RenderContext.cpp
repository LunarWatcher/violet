#include "RenderContext.hpp"
#include "violet/data/VioletGlobal.hpp"

namespace violet {

ListingRenderContext::operator iris::DataObject() const {
    return iris::Object {
        {"page", page},
        {"total_pages", total_pages},
        {"pages", pages},
        {"base_path", base_path.string()},
    };
}

RenderContext::operator iris::DataObject() const {
    iris::Object obj {
        {"page", this->fm},
        {"site", site},
        {"violet", violet::violet_global}
    };

    if (listing.has_value()) {
        obj["listing"] = *listing;
    }

    return obj;
}

}
