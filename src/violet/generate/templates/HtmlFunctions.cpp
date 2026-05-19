#include "HtmlFunctions.hpp"

#include "InjaManager.hpp"
#include "violet/parsing/Escaping.hpp"

namespace violet {

HtmlFunctions::HtmlFunctions(InjaManager& man) : man(man) {
    man.env.add_callback("htmlEscape", 1, [this](inja::Arguments& args) -> nlohmann::json {
        return this->htmlEscape(args);
    });
}

nlohmann::json HtmlFunctions::htmlEscape(inja::Arguments& args) {
    std::stringstream ss;

    for (auto& ch : args.at(0)->get<std::string>()) {
        violet::htmlEscape(ch, ss, true);
    }

    return ss.str();
}

}
