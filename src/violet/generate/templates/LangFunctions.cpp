#include "LangFunctions.hpp"
#include "iris/function_storage.hpp"
#include "violet/generate/templates/IrisManager.hpp"

namespace violet {

LangFunctions::LangFunctions(IrisManager& man) : man(man) {
    man.env.add_callback("recurseInclude", 2, [this](iris::Arguments& args) {
        return recurseInclude(args);
    });
    man.env.add_callback("createPaginatedList", 2, [this](iris::Arguments& args) {
        return createPaginatedList(args);
    });
}

iris::DataObject LangFunctions::recurseInclude(iris::Arguments& args) {
    auto templateName = args.at(0)->get<std::string>();
    auto& context = *args.at(1);
    std::stringstream ss;
    ss << "{% include " << std::quoted(templateName) << " %}";

    return this->man.env.render(
        ss.str(),
        context
    );
}

iris::DataObject LangFunctions::createPaginatedList(iris::Arguments& args) {
    int64_t rCurrPage = args.at(0)->get<iris::Int>();
    int64_t rTotalPages = args.at(1)->get<iris::Int>();

    if (rCurrPage <= 0) {
        throw std::runtime_error("currPage must be strictly greater than 0");
    } else if (rTotalPages <= 0) {
        throw std::runtime_error("totalPages must be strictly greater than 0");
    }

    uint64_t currPage = rCurrPage;
    uint64_t totalPages = rTotalPages;

    // This is not an elegant way to construct the pages lists, but it's good enough
    std::vector<uint64_t> sourcePages = {
        1, 2, 3,
        currPage - 1, currPage, currPage + 1,
        // Need to avoid underflow. totalPages >= 1, but not necessarily >= 2
        (totalPages > 2 ? totalPages - 2 : 0), totalPages - 1, totalPages
    };

    std::vector<iris::DataObject> pages;

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
