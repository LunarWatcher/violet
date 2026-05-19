
#pragma once

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

namespace violet {

class InjaManager;
class HtmlFunctions {
private:
    InjaManager& man;

    nlohmann::json htmlEscape(inja::Arguments& args);
public:
    HtmlFunctions(InjaManager& man);
};

}
