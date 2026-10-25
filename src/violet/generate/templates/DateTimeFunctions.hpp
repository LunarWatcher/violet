#pragma once

#include <nlohmann/json.hpp>
#include <iris/iris.hpp>

namespace violet {

class IrisManager;
class DateTimeFunctions {
private:
    IrisManager& man;

    iris::DataObject formatDate(iris::Arguments& args);
    iris::DataObject parseDate(iris::Arguments& args);
public:
    DateTimeFunctions(IrisManager& man);
};

}
