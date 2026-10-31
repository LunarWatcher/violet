#pragma once

#include "violet/generate/templates/types/RenderContext.hpp"
#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class LangFunctions {
private:
    IrisManager& man;

    iris::DataObject recurseInclude(iris::Arguments& args, RenderContext* context);
    iris::DataObject createPaginatedList(iris::Arguments& args);
public:
    LangFunctions(IrisManager& man);
};

}
