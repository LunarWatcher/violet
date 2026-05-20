#pragma once

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

namespace violet {

class InjaManager;
class DateTimeFunctions {
private:
    InjaManager& man;

    nlohmann::json formatDate(inja::Arguments& args);
public:
    DateTimeFunctions(InjaManager& man);
};

}
