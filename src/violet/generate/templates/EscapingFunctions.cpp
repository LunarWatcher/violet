#include "EscapingFunctions.hpp"

#include "IrisManager.hpp"
#include "violet/parsing/Escaping.hpp"

namespace violet {

EscapingFunctions::EscapingFunctions(IrisManager& man) : man(man) {
    man.env.add_callback("htmlEscape", 1, [this](iris::Arguments& args) -> nlohmann::json {
        return this->htmlEscape(args);
    });
    man.env.add_callback("urlEncode", 1, [this](iris::Arguments& args) -> nlohmann::json {
        return this->urlEncode(args);
    });
    man.env.add_callback("urlEncode", 2, [this](iris::Arguments& args) -> nlohmann::json {
        return this->urlEncode(args);
    });
}

nlohmann::json EscapingFunctions::htmlEscape(iris::Arguments& args) {
    std::stringstream ss;

    for (auto& ch : args.at(0)->get<std::string>()) {
        violet::htmlEscape(ch, ss, true);
    }

    return ss.str();
}

nlohmann::json EscapingFunctions::urlEncode(iris::Arguments& args) {
    std::stringstream ss;

    violet::urlEncode(
        args.at(0)->get<std::string>(),
        ss,
        args.size() == 2 ? args.at(1)->get<bool>() : true
    );
    return ss.str();
}

}
