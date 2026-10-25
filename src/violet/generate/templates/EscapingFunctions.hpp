
#pragma once

#include <nlohmann/json.hpp>
#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class EscapingFunctions {
private:
    IrisManager& man;

    iris::DataObject htmlEscape(iris::Arguments& args);
    iris::DataObject urlEncode(iris::Arguments& args);
public:
    EscapingFunctions(IrisManager& man);
};

}
