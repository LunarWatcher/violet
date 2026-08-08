#include "EscapingFunctions.hpp"

#include "InjaManager.hpp"
#include "violet/parsing/Escaping.hpp"

namespace violet {

EscapingFunctions::EscapingFunctions(InjaManager& man) : man(man) {
    man.env.add_callback("htmlEscape", 1, [this](inja::Arguments& args) -> nlohmann::json {
        return this->htmlEscape(args);
    });
    man.env.add_callback("urlEncode", 1, [this](inja::Arguments& args) -> nlohmann::json {
        return this->urlEncode(args);
    });
    man.env.add_callback("urlEncode", 2, [this](inja::Arguments& args) -> nlohmann::json {
        return this->urlEncode(args);
    });
}

nlohmann::json EscapingFunctions::htmlEscape(inja::Arguments& args) {
    std::stringstream ss;

    for (auto& ch : args.at(0)->get<std::string>()) {
        violet::htmlEscape(ch, ss, true);
    }

    return ss.str();
}

nlohmann::json EscapingFunctions::urlEncode(inja::Arguments& args) {
    std::stringstream ss;

    violet::urlEncode(
        args.at(0)->get<std::string>(),
        ss,
        args.size() == 2 ? args.at(1)->get<bool>() : true
    );
    return ss.str();
}

}
