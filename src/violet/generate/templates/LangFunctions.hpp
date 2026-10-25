#pragma once

#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class LangFunctions {
private:
    IrisManager& man;

    iris::DataObject recurseInclude(iris::Arguments& args);
    iris::DataObject createPaginatedList(iris::Arguments& args);
public:
    LangFunctions(IrisManager& man);
};

}
