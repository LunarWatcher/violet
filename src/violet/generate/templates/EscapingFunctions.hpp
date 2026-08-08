
#pragma once

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

namespace violet {

class InjaManager;
class EscapingFunctions {
private:
    InjaManager& man;

    nlohmann::json htmlEscape(inja::Arguments& args);
    nlohmann::json urlEncode(inja::Arguments& args);
public:
    EscapingFunctions(InjaManager& man);
};

}
