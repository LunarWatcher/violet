#pragma once

#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class FileFunctions {
private:
    IrisManager& man;
public:
    FileFunctions(IrisManager& man);

    iris::DataObject listPages(iris::Arguments& args);
    iris::DataObject treePages(iris::Arguments& args);

    iris::DataObject listPagesByTaxonomy(iris::Arguments& args);

    iris::DataObject listPagesPaginated(iris::Arguments& args);

    iris::DataObject paginatedUrl(iris::Arguments& args);

    iris::DataObject loadJson(iris::Arguments& args);
    iris::DataObject render(iris::Arguments& args);
};

}
