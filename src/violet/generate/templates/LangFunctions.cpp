#include "LangFunctions.hpp"
#include "violet/generate/templates/InjaManager.hpp"

namespace violet {

LangFunctions::LangFunctions(InjaManager& man) : man(man) {
    man.env.add_callback("recurseInclude", 2, [this](inja::Arguments& args) {
        return recurseInclude(args);
    });
}

nlohmann::json LangFunctions::recurseInclude(inja::Arguments& args) {
    auto templateName = args.at(0)->get<std::string>();
    auto& context = *args.at(1);
    std::stringstream ss;
    ss << "{% include " << std::quoted(templateName) << " %}";

    return this->man.env.render(
            ss.str(),
            context
    );
}

}
