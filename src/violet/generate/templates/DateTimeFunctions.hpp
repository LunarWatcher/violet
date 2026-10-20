#pragma once

#include <nlohmann/json.hpp>
#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class DateTimeFunctions {
private:
    IrisManager& man;

    nlohmann::json formatDate(iris::Arguments& args);
public:
    DateTimeFunctions(IrisManager& man);
};

}
