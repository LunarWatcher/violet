
#pragma once

#include <nlohmann/json.hpp>
#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class EscapingFunctions {
private:
    IrisManager& man;

    nlohmann::json htmlEscape(iris::Arguments& args);
    nlohmann::json urlEncode(iris::Arguments& args);
public:
    EscapingFunctions(IrisManager& man);
};

}
