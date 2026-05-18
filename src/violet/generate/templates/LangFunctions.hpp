#pragma once

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

namespace violet {

class InjaManager;
class LangFunctions {
private:
    InjaManager& man;

    nlohmann::json recurseInclude(inja::Arguments& args);

public:
    LangFunctions(InjaManager& man);
};

}
