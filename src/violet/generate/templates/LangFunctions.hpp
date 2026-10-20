#pragma once

#include <nlohmann/json.hpp>
#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class LangFunctions {
private:
    IrisManager& man;

    nlohmann::json recurseInclude(iris::Arguments& args);
    nlohmann::json createPaginatedList(iris::Arguments& args);
public:
    LangFunctions(IrisManager& man);
};

}
