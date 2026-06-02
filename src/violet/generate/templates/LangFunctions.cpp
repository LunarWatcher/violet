#include "LangFunctions.hpp"
#include "inja/function_storage.hpp"
#include "violet/generate/templates/InjaManager.hpp"

namespace violet {

LangFunctions::LangFunctions(InjaManager& man) : man(man) {
    man.env.add_callback("recurseInclude", 2, [this](inja::Arguments& args) {
        return recurseInclude(args);
    });
    man.env.add_callback("createPaginatedList", 2, [this](inja::Arguments& args) {
        return createPaginatedList(args);
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

nlohmann::json LangFunctions::createPaginatedList(inja::Arguments& args) {
    uint64_t currPage = *args.at(0);
    uint64_t totalPages = *args.at(1);

    if (currPage == 0) {
        throw std::runtime_error("currPage must be strictly greater than 0");
    } else if (totalPages == 0) {
        throw std::runtime_error("totalPages must be strictly greater than 0");
    }

    // This is not an elegant way to construct the pages lists, but it's good enough
    std::vector<uint64_t> sourcePages = {
        1, 2, 3,
        currPage - 1, currPage, currPage + 1,
        // Need to avoid underflow. totalPages >= 1, but not necessarily >= 2
        (totalPages > 2 ? totalPages - 2 : 0), totalPages - 1, totalPages
    };

    std::vector<uint64_t> pages;

    for (auto page : sourcePages) {
        if (page == 0
            || page > totalPages
            || std::find(
                pages.begin(), pages.end(), page
            ) != pages.end()
        ) {
            continue;
        }

        pages.push_back(page);
    }

    return pages;
}

}
