#pragma once

#include "violet/algorithm/SortMethod.hpp"
#include "violet/generate/templates/types/RenderContext.hpp"
#include <compare>
#include <iris/iris.hpp>

namespace violet {

struct SingleSortCacheKey {
    SortMethod method;
    std::string resolvedScope;

    std::strong_ordering operator<=>(const SingleSortCacheKey& other) const {
        auto i = (method <=> other.method);
        if (i == 0) {
            i = (resolvedScope <=> other.resolvedScope);
        }
        return i;
    }
};
struct DualSortCacheKey {
    SortMethod leafMethod;
    SortMethod nodeMethod;
    std::string resolvedScope;

    std::strong_ordering operator<=>(const DualSortCacheKey& other) const {
        auto i = (nodeMethod <=> other.nodeMethod);
        if (i == 0) {
            i = (leafMethod <=> other.leafMethod);
            if (i == 0) {
                i = (resolvedScope <=> other.resolvedScope);
            }
        }
        return i;
    }
};

class IrisManager;
class FileFunctions {
private:
    IrisManager& man;

    std::map<DualSortCacheKey, iris::DataObject> treePagesCache;
    std::map<SingleSortCacheKey, iris::DataObject> listPagesCache;
public:
    FileFunctions(IrisManager& man);

    iris::DataObject listPages(iris::Arguments& args, RenderContext* ctx);
    iris::DataObject treePages(iris::Arguments& args, RenderContext* ctx);

    iris::DataObject listPagesByTaxonomy(iris::Arguments& args);

    iris::DataObject listPagesPaginated(iris::Arguments& args, RenderContext* ctx);

    iris::DataObject paginatedUrl(iris::Arguments& args);

    iris::DataObject abspath(iris::Arguments& args, RenderContext* ctx);

    iris::DataObject loadJson(iris::Arguments& args, RenderContext* ctx);
    iris::DataObject render(iris::Arguments& args);

    void clear();
};

}
