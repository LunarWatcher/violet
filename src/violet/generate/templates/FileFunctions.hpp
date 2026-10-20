#pragma once

#include <nlohmann/json.hpp>
#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class FileFunctions {
private:
    IrisManager& man;
public:
    FileFunctions(IrisManager& man);

    nlohmann::json listPages(iris::Arguments& args);
    nlohmann::json treePages(iris::Arguments& args);

    nlohmann::json listPagesByTaxonomy(iris::Arguments& args);

    nlohmann::json listPagesPaginated(iris::Arguments& args);

    nlohmann::json paginatedUrl(iris::Arguments& args);

    nlohmann::json loadJson(iris::Arguments& args);
    nlohmann::json render(iris::Arguments& args);
};

}
