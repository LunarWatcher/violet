#pragma once

#include "violet/generate/templates/types/RenderContext.hpp"
#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class FileFunctions {
private:
    IrisManager& man;
public:
    FileFunctions(IrisManager& man);

    iris::DataObject listPages(iris::Arguments& args, RenderContext* ctx);
    iris::DataObject treePages(iris::Arguments& args, RenderContext* ctx);

    iris::DataObject listPagesByTaxonomy(iris::Arguments& args, RenderContext* ctx);

    iris::DataObject listPagesPaginated(iris::Arguments& args, RenderContext* ctx);

    iris::DataObject paginatedUrl(iris::Arguments& args);

    iris::DataObject loadJson(iris::Arguments& args, RenderContext* ctx);
    iris::DataObject render(iris::Arguments& args);
};

}
